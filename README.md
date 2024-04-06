# esp-idf-mqtt2bt
MQTT to Bluetooth bridge for ESP-IDF

![classic-2](https://github.com/nopnop2002/esp-idf-mqtt2bt/assets/6020549/30d65ad1-6ba6-4930-9acd-2b69cf9c87ef)

![mqtt2bt](https://github.com/nopnop2002/esp-idf-mqtt2bt/assets/6020549/ce1d70f7-ca03-44e4-8062-449e88d6b7ec)


# Software requirements
ESP-IDF V4.4/V5.x.   
ESP-IDF V5.1 is required when using ESP32-C6.   


# Using classic bluetooth (ESP32 only)

```
git clone https://github.com/nopnop2002/esp-idf-mqtt2bt
cd esp-idf-mqtt2bt/classic_bt
idf.py set-target esp32
idf.py menuconfig
idf.py flash
```


# Using ble 4.2

```
git clone https://github.com/nopnop2002/esp-idf-mqtt2bt
cd esp-idf-mqtt2bt/ble
idf.py set-target {esp32/esp32s3/esp32c3/esp32c6}
idf.py menuconfig
idf.py flash
```

__Note for ESP32C2__   
It cannot be executed because the RAM is small.


# Configuration
![config-top](https://github.com/nopnop2002/esp-idf-mqtt2bt/assets/6020549/5d10c7ca-473d-4d06-8a5b-c436691c9fb5)
![config-app](https://github.com/nopnop2002/esp-idf-mqtt2bt/assets/6020549/758fc8e0-929a-47fa-8183-b8f1b12b1c5b)

## Wifi Setting
![config-wifi](https://github.com/nopnop2002/esp-idf-mqtt2bt/assets/6020549/91bd776e-32ad-4873-8ace-d7d7d0cde445)

## MQTT Setting
MQTT broker is specified by one of the following.
- IP address   
 ```192.168.10.20```   
- mDNS host name   
 ```mqtt-broker.local```   
- Fully Qualified Domain Name   
 ```broker.emqx.io```

You can download the MQTT broker from [here](https://github.com/nopnop2002/esp-idf-mqtt-broker).   

Wildcards can be used in Subscribe topic.   

![config-broker-1](https://github.com/nopnop2002/esp-idf-mqtt2bt/assets/6020549/527f1b35-56f4-4a48-aaba-3771535c88cf)

Specifies the username and password if the server requires a password when connecting.   
[Here's](https://www.digitalocean.com/community/tutorials/how-to-install-and-secure-the-mosquitto-mqtt-messaging-broker-on-debian-10) how to install and secure the Mosquitto MQTT messaging broker on Debian 10.   

![config-broker-2](https://github.com/nopnop2002/esp-idf-mqtt2bt/assets/6020549/7593b28f-d777-4e14-99cb-5feea4936aa4)

# MQTT Publish
You can use ```mqtt_pub.sh```.

# MQTT Subscrinbe
You can use ```mqtt_sub.sh```.

# Android Application   
I used [this](https://play.google.com/store/apps/details?id=de.kai_morich.serial_bluetooth_terminal) app.   


# iOS Application   
[This](https://apps.apple.com/jp/app/bluetooth-v2-1-spp-setup/id6449416841) might work, but I don't have iOS so I don't know.   

