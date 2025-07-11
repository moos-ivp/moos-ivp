#!/bin/bash

echo "Launching Shoreside..."
pAntler targ_shoreside.moos > shoreside.log 2>&1 &

echo "Launching Vehicle..."
pAntler targ_vehicle.moos > vehicle.log 2>&1 &

sleep 1
echo "Processes launched. Check shoreside.log and vehicle.log for issues."
