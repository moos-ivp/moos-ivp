#!/bin/bash -e
#---------------------------------------------------------
# File: launch.sh
# Name: Mike Benjamin
# Date: May 14th, 2019
# Note: Baseline sim launch for Front Estimation baseline
#---------------------------------------------------------
#  Part 1: Initialize configurable variables with defaults
#---------------------------------------------------------
TIME_WARP=""
JUST_MAKE=""

#-------------------------------------------------------
#  Part 2: Check for and handle command-line arguments
#-------------------------------------------------------
for ARGI; do
    if [ "${ARGI}" = "--help" -o "${ARGI}" = "-h" ] ; then
	echo "$0 [SWITCHES]"
	echo "  --help, -h         Show this help message          "
	echo "  --just_make, -j    Just make targ files, no launch "
	exit 0;
    elif [ "${ARGI//[^0-9]/}" = "$ARGI" -a "$TIME_WARP" = "" ]; then 
        TIME_WARP=$ARGI
    elif [ "${ARGI}" = "--just_build" -o "${ARGI}" = "-j" ] ; then
        JUST_MAKE=$ARGI
    else 
        echo "Bad arg:" $ARGI "Run with -h for help."
        echo "The launch.sh script is exiting with (1)."
        exit 1
    fi
done


#-------------------------------------------------------
#  Part 3: Launch both vehicle communities
#-------------------------------------------------------
COMMON_ARGS="--sim --noui $TIME_WARP $JUST_MAKE --shoreip=localhost"

./launch_vehicle.sh -v5 $COMMON_ARGS --degrees=270
./launch_vehicle.sh -v6 $COMMON_ARGS --degrees=0


#-------------------------------------------------------
#  Part 4: Launch the shoreside MOOS community
#-------------------------------------------------------
echo "Launching Shoreside...."
./launch_shoreside.sh $TIME_WARP $JUST_MAKE


