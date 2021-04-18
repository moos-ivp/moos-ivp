#!/bin/bash -e
#----------------------------------------------------------
#  Script: launch.sh
#  Author: Michael Benjamin
#  LastEd: Feb 27th 2021
#----------------------------------------------------------
#  Part 1: Set Exit actions and declare global var defaults
#----------------------------------------------------------
TIME_WARP=1
JUST_MAKE="no"
DRESET="false"
POINTS="false"
OBAVD="avoidx"

#----------------------------------------------------------
#  Part 2: Check for and handle command-line arguments
#----------------------------------------------------------
for ARGI; do
    if [ "${ARGI}" = "--help" -o "${ARGI}" = "-h" ] ; then
	echo "launch.sh [SWITCHES] [time_warp]                   " 
	echo "  --help, -h                                       "
	echo "    Show this help message                         " 
	echo "  --just_make, -j                                  "
	echo "    Just create targ files, no launch " 
	echo "  --dynamic_reset, -d                              " 
	echo "    Enable dynamice resetting of obstacles in the  "
	echo "    obstacle simulator                             "
	echo "  --points, -p                                     " 
	echo "    Enable the simulated sensor points mode in the "
	echo "    obstacle simulator                             "
	echo "  --old_avd, -o                                    "
	echo "    Enable the older AvoidObstacles obstacle       "
	echo "    avoidance behavior, not newer AvoidObstaclesX  "
	exit 0;
    elif [ "${ARGI//[^0-9]/}" = "$ARGI" -a "$TIME_WARP" = 1 ]; then 
        TIME_WARP=$ARGI
    elif [ "${ARGI}" = "--just_make" -o "${ARGI}" = "-j" ]; then
	JUST_MAKE="yes"
    elif [ "${ARGI}" = "--dynamic_reset" -o "${ARGI}" = "-d" ]; then
	DRESET="true"
    elif [ "${ARGI}" = "--points" -o "${ARGI}" = "-p" ]; then
	POINTS="true"
    elif [ "${ARGI}" = "--old_avd" -o "${ARGI}" = "-o" ]; then
	OBAVD="old"
    else 
        echo "launch.sh Bad arg:" $ARGI " Exiting with code: 1"
        exit 1
    fi
done

#-------------------------------------------------------
#  Part 3: Create the .moos and .bhv files. 
#-------------------------------------------------------
VNAME1="henry"           # The first vehicle Community

START_POS1="0,-80,90"         

LOITER_POS1="x=0,y=-75"
VCOLOR1="light_green"

SHORE_PSHARE="9200"

nsplug meta_vehicle.moos targ_henry.moos -i -f WARP=$TIME_WARP \
       VNAME=$VNAME1            BOT_PSHARE="9201"              \
       BOT_MOOSDB="9001"        SHORE_PSHARE=$SHORE_PSHARE     \
       START_POS=$START_POS1    VCOLOR=$VCOLOR1

nsplug meta_shoreside.moos targ_shoreside.moos -i -f WARP=$TIME_WARP \
       SHORE_PSHARE=$SHORE_PSHARE  SHORE_MOOSDB="9000"        \
       DRESET=$DRESET              POINTS=$POINTS

nsplug meta_vehicle.bhv targ_henry.bhv -i -f VNAME=$VNAME1    \
       START_POS=$START_POS1   LOITER_POS=$LOITER_POS1        \
       OBAVD=$OBAVD 

if [ ${JUST_MAKE} = "yes" ] ; then
    echo "Files assembled; nothing launched; exiting per request."
    exit 0
fi

#----------------------------------------------------------
#  Part 4: Launch the processes
#----------------------------------------------------------
echo "Launching Shoreside MOOS Community. WARP is" $TIME_WARP
pAntler targ_shoreside.moos >& /dev/null &

echo "Launching $VNAME1 MOOS Community. WARP is" $TIME_WARP
pAntler targ_henry.moos >& /dev/null &

uMAC --proc=pObstacleMgr --node=gilda targ_shoreside.moos

kill -- -$$
