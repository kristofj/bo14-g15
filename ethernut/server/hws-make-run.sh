#!/bin/bash
#hws-make-run.sh
#Passer på at HessdalenWeatherServer kjører.

program=HessdalenWeatherServer

if ps aux | grep -v grep | grep $program
	then
		exit
	else
		java $program &
	fi
exit

