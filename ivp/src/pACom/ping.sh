#!/bin/bash

SERIAL_PORT="/dev/ttyS2"
START_PING="\$CCMPC"
CONF_PING="\$CAMPC,1,2"
REC_PING="\$CAMPR,2,1,3.13"
PING_INTERVAL=0.5
#while true; do
#  read BUFFER < $SERIAL_PORT
#  sleep 1
  
#  echo $BUFFER
  
#  CHARS=$(expr match "$BUFFER" '\$CCMPC')
  
#  if [[ CHARS -eq 6 ]]; then
#    echo $CONF_PING > $SERIAL_PORT
#    echo "confirmation sent"

#    sleep 3
#    echo $REC_PING > $SERIAL_PORT
#    echo "ping sent"
#  fi
  
#  sleep $PING_INTERVAL
#done

while true
do
  read LINE < $SERIAL_PORT
  echo ---------------
  echo $LINE
  echo ---------------
  sleep $PING_INTERVAL
done
