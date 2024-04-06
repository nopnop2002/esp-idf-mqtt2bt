#!/bin/bash
#set -x
fail=0
while :
do
	payload=`date "+%Y/%m/%d %H:%M:%S"`
	echo ${payload}
	mosquitto_pub -h broker.emqx.io -p 1883 -t "/topic/subscribe" -m "${payload}"
	if [ $? -ne 0 ]; then
		fail=$((++fail))
		echo ${fail}
		sleep 3
	fi
	sleep 1
done
