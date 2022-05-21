#!/bin/bash

IP=192.168.1.139

cd /mnt/c/Program\ Files/mosquitto/

while true 
do  

     ./mosquitto_sub.exe -h "192.168.1.139" -t "th" | while read -r payload 
    do
        # echo "Writing Temperature: ${payload} to file"
        # ${payload} C:\temp >> tempdata.txt
        if [[ ${payload} -gt 70 ]]; then
        echo "turn on your AC" 
        fi
    done
    sleep 10 
done 