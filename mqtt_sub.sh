#!/bin/bash
mosquitto_sub -h broker.emqx.io -p 1883 -t "/topic/publish" | ts "%y/%m/%d %H:%M:%S"
#mosquitto_sub -h mqtt-server.local -p 1883 -t "/topic/publish" -u user -P password | ts "%y/%m/%d %H:%M:%S"
