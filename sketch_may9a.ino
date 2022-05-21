#include <WiFi.h>
#include <PubSubClient.h>
#include <LiquidCrystal_I2C.h>
#include "DHTesp.h"

#define SDA 13 //Define SDA pins
#define SCL 14 //Define SCL pins
DHTesp dht; // create dht object
LiquidCrystal_I2C lcd(0x27,16,2); //initialize the LCD
int dhtPin = 18; // the number of the DHT11 sensor pin

// WiFi
const char *ssid = "MySpectrumWiFi70-2G";
const char *password = "chillyhat810"; 

// MQTT Broker
const char *mqtt_broker = "192.168.1.139";
const char *topic = "th";
const char *mqtt_username = "";
const char *mqtt_password = "";
const int mqtt_port = 1883;

WiFiClient espClient;
PubSubClient client(espClient);

long lastReconnectAttempt = 0;

boolean reconnect() {
  if (client.connect("arduinoClient")) {
    client.publish("outTopic","hello world");
    client.subscribe("inTopic");
  }
  return client.connected();
}

void setup() {

 Serial.begin(115200);
 // connecting to a WiFi network
 WiFi.begin(ssid, password);
 while (WiFi.status() != WL_CONNECTED) {
     delay(500);
     Serial.println("Connecting to WiFi..");
 }
 Serial.println("Connected to the WiFi network");
 //connecting to a mqtt broker
 client.setServer(mqtt_broker, mqtt_port);
 client.setCallback(callback);
 while (!client.connected()) {
     String client_id = "esp32-client-";
     client_id += String(WiFi.macAddress());
     Serial.printf("The client %s connects to the public mqtt broker\n", client_id.c_str());
     if (client.connect(client_id.c_str(), mqtt_username, mqtt_password)) {
         Serial.println("Public emqx mqtt broker connected");
     } else {
         Serial.print("failed with state ");
         Serial.print(client.state());
         delay(2000);
     }
 }

 client.publish(topic, "Hi EMQX I'm ESP32");
 client.subscribe(topic);

 Wire.begin(SDA, SCL); 
 lcd.init(); 
 lcd.backlight(); 
 dht.setup(dhtPin, DHTesp::DHT11); 

}

void callback(char *topic, byte *payload, unsigned int length) {
 Serial.print("Message arrived in topic: ");
 Serial.println(topic);
 Serial.print("Message:");
 for (int i = 0; i < length; i++) {
     Serial.print((char) payload[i]);
 }
 Serial.println();
 Serial.println("-----------------------");
}

void loop() {

client.loop();

if (!client.connected()) {
    long now = millis();
    if (now - lastReconnectAttempt > 5000) {
      lastReconnectAttempt = now;
      if (reconnect()) {
        lastReconnectAttempt = 0;
      }
    }
  }
 
 flag:TempAndHumidity DHT = dht.getTempAndHumidity();
 float t = 0;
 if (dht.getStatus()  != 0) { 
 goto flag;
 } 
 t = DHT.temperature;
 t = t*9/5 + 32;
 char buffer[10];
 dtostrf(t,0, 0, buffer);
 client.publish(topic, buffer);
 lcd.setCursor(0, 0);
 lcd.print("Temperature:"); 
 lcd.print((DHT.temperature)*9/5 +32); 
 lcd.setCursor(0, 1); 
 lcd.print("Humidity :");
 lcd.print(DHT.humidity); 
 delay(5000);
}
