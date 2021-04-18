#!/bin/bash -e
#--------------------------------------------------------------
#   Script: launch_shoreside.sh                                    
#   Author: Michael Benjamin  
#     Date: April 2020     
#--------------------------------------------------------------  
#  Part 1: Set Exit actions and declare global var defaults
#--------------------------------------------------------------
trap "kill -- -$$" EXIT SIGTERM SIGHUP SIGINT SIGKILL

TIME_WARP=1
JUST_MAKE=""
PASS_ARGS=""

#--------------------------------------------------------------  
#  Part 2: Check for and handle command-line arguments
#--------------------------------------------------------------  
for ARGI; do
    if [ "${ARGI}" = "--help" -o "${ARGI}" = "-h" ] ; then
	echo "launch.sh [SWITCHES] [time_warp]    "
	echo "  --help, -h                       " 
	echo "  --just_make, -j                  " 
	echo "  --pavlab, -p                     "
	echo "    Set region to be MIT pavlab    " 
	exit 0;
    elif [ "${ARGI//[^0-9]/}" = "$ARGI" -a "$TIME_WARP" = 1 ]; then 
        TIME_WARP=$ARGI
    elif [ "${ARGI}" = "--just_make" -o "${ARGI}" = "-j" ] ; then
	JUST_MAKE="yes"
    elif [ "${ARGI}" = "--pavlab" -o "${ARGI}" = "-p" ]; then
        PASS_ARGS+=$ARGI
    else 
        echo "launch.sh Bad arg:" $ARGI " Exiting with code: 1"
        exit 1
    fi
done


#--------------------------------------------------------------  
#  Part 3: Pre-launch. Better to exit now if err building targs
#--------------------------------------------------------------  
./launch_shoreside.sh -j --auto $PASS_ARGS $TIME_WARP
./launch_vehicle.sh   -j --auto $PASS_ARGS $TIME_WARP --vname=abe

if [ ${JUST_MAKE} = "yes" ] ; then
    exit 0
fi

#--------------------------------------------------------------  
#  Part 4: Actual launch
#--------------------------------------------------------------  
./launch_shoreside.sh --auto $PASS_ARGS $TIME_WARP
sleep 1
./launch_vehicle.sh   --auto $PASS_ARGS $TIME_WARP --vname=abe

#--------------------------------------------------------------  
#  Part 5: Launch uMAC until mission quit
#--------------------------------------------------------------  
uMAC targ_shoreside.moos
