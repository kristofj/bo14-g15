#!/bin/bash
#hws-make-run.sh
#Passer på at HessdalenWeatherServer kjører.

program=hessdalen16.jar

if ps aux | grep -v grep | grep $program > /dev/null
then
	exit
else
        java -jar $program &
fi
exit
