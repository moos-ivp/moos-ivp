#!/bin/bash 
#----------------------------------------------------------
#  Script: xlaunch_res.sh
#  Author: Michael Benjamin
#  LastEd: August 2020
#----------------------------------------------------------
#  Part 1: Set Exit actions and declare global var defaults
#----------------------------------------------------------
TIME_WARP=1
JUST_MAKE="no"
FLOW_DOWN_ARGS="--auto "
RES_FILE=""
TNUM=""

#-------------------------------------------------------
#  Part 2: Check for and handle command-line arguments
#-------------------------------------------------------
for ARGI; do
    if [ "${ARGI}" = "--help" -o "${ARGI}" = "-h" ] ; then
	echo "xlaunch.sh [SWITCHES] [time_warp]     "
	echo "  --help, -h                          " 
	echo "  --just_make, -j                     " 
	exit 0;
    elif [ "${ARGI//[^0-9]/}" = "$ARGI" -a "$TIME_WARP" = 1 ]; then 
	TIME_WARP=$ARGI
    elif [ "${ARGI}" = "--just_make" -o "${ARGI}" = "-j" ]; then
	FLOW_DOWN_ARGS+="${ARGI} "
	JUST_MAKE="yes"
    elif [ "${ARGI:0:3}" = "--test=" ]; then
	FLOW_DOWN_ARGS+="${ARGI} "
        TNUM="${ARGI#--test=*}"
    elif [ "${ARGI:0:10}" = "--resfile=" ]; then
        RESFILE="${ARGI#--resfile=*}"
    else 
	FLOW_DOWN_ARGS+="${ARGI} "
    fi
done

#-------------------------------------------------------
#  Part 3: Launch mission
#-------------------------------------------------------
./launch.sh $FLOW_DOWN_ARGS $TIME_WARP
LEXIT_CODE=$?

if [ $LEXIT_CODE != 0 ]; then
    echo "The launch.sh cmd return non-zero exit code: " $LEXIT_CODE
    exit 1
fi
if [ ${JUST_MAKE} = "yes" ] ; then
    exit 0
fi

#-------------------------------------------------------
#  Part 4: Start the mission with the right pokes
#-------------------------------------------------------
echo "Poking (Starting the mission) in 4 seconds ...." 
sleep 4
uPokeDB targ_shoreside.moos DEPLOY_ALL=true MOOS_MANUAL_OVERRIDE_ALL=false

#-------------------------------------------------------
#  Part 5: Monitor mission, kill MOOS processes when done
#-------------------------------------------------------
while [ 1 ]; do 
    sleep 6
    if uQueryDB targ_shoreside.moos ; then 
        break;
    fi
    echo After uQueryDB, waiting to try again...
done

echo "Checking for Pass/Fail....."
uQueryDB --passfail targ_shoreside.moos
echo "Done Checking for Pass/Fail....."


#-------------------------------------------------------
#  Part 6: If reporting results, build results file
#-------------------------------------------------------
if [ ${RESFILE} = "" ] ; then
    exit 0
fi



