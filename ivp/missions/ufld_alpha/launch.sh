#!/bin/bash -e
#--------------------------------------------------------------
#   Script: launch_shoreside.sh                                    
#   Author: Michael Benjamin  
#     Date: April 2020     
#--------------------------------------------------------------
#----------------------------------------------------------  
#  Part 1: Set Exit actions and declare global var defaults
#----------------------------------------------------------
trap "kill -- -$$" EXIT SIGTERM SIGHUP SIGINT SIGKILL

TIME_WARP=1
JUST_MAKE=""

#-------------------------------------------------------
#  Part 2: Check for and handle command-line arguments
#-------------------------------------------------------
for ARGI; do
    if [ "${ARGI}" = "--help" -o "${ARGI}" = "-h" ] ; then
	echo"launch.sh [SWITCHES] [time_warp]   "
	echo"  --just_make, -j                  " 
	echo"  --help, -h                       " 
	exit 0;
    elif [ "${ARGI//[^0-9]/}" = "$ARGI" -a "$TIME_WARP" = 1 ]; then 
        TIME_WARP=$ARGI
    elif [ "${ARGI}" = "--just_make" -o "${ARGI}" = "-j" ] ; then
	JUST_MAKE="-j"
    else 
	echo"launch.sh: Bad Arg:" $ARGI
	exit 0
    fi
done

./launch_shoreside.sh --auto $JUST_MAKE $TIME_WARP &
sleep 1
./launch_vehicle.sh --auto $JUST_MAKE $TIME_WARP --vname=abe &

uMAC targ_shoreside.moos

