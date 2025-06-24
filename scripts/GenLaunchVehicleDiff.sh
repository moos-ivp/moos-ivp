#!/bin/bash
#--------------------------------------------------
#   Script: GenlaunchVehicleDiff.sh
#   Author: M.Benjamin
#   LastEd: May 2024
#--------------------------------------------------
for ARGI; do
    CMD_ARGS+="${ARGI} "
done

FILE="launch_vehicle.sh"

GenLaunchVehicle.sh $CMD_ARGS --output=tmp_$FILE
diff -w -B $FILE tmp_$FILE | fgrep -v Author | fgrep -v LastEd
rm -f tmp_$FILE

exit 0
