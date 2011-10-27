#!/bin/sh

MOOSDB &> /dev/null &
iPWMController /home/mikerb/Research/moos-ivp/MOOSBin/iPWMController.moos &

sleep 2

iRemote

killall iPWMController
killall MOOSDB
