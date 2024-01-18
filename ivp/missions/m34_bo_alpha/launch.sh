#!/bin/bash -e
#----------------------------------------------------------
#  Script: launch.sh
#  Author: Michael Benjamin
#  LastEd: June 3rd 2020
#----------------------------------------------------------
#  Part 1: Set Exit actions and declare global var defaults
#----------------------------------------------------------
TIME_WARP=1
JUST_MAKE="no"
DRESET="false"
POINTS="false"
COLAVD="colregs"
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
	echo "  --cpa, -c                                        " 
	echo "    Enable the CPA collision avoidance behavior    "
	echo "    instead of the COLREGS based avoid behavior    "
	echo "  --old_avd, -o                                    "
	echo "    Enable the older AvoidObstacles obstacle       "
	echo "    avoidance behavior, not newer AvoidObstaclesX  "
	exit 0;
    elif [ "${ARGI//[^0-9]/}" = "$ARGI" -a "$TIME_WARP" = 1 ]; then 
        TIME_WARP=$ARGI
    elif [ "${ARGI}" = "--just_make" -o "${ARGI}" = "-j" ] ; then
	JUST_MAKE="yes"
    elif [ "${ARGI}" = "--dynamic_reset" -o "${ARGI}" = "-d" ] ; then
	DRESET="true"
    elif [ "${ARGI}" = "--points" -o "${ARGI}" = "-p" ] ; then
	POINTS="true"
    elif [ "${ARGI}" = "--cpa" -o "${ARGI}" = "-c" ] ; then
	COLAVD="cpa"
    elif [ "${ARGI}" = "--old_avd" -o "${ARGI}" = "-o" ] ; then
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
VNAME2="gilda"           # The second vehicle Community
#START_POS1="0,0"         
#START_POS2="80,0"        

START_POS1="-10,-30,180"         
START_POS2="150,-130,0"        

LOITER_POS1="x=0,y=-75"
LOITER_POS2="x=145,y=-70"
VCOLOR1="light_green"
VCOLOR2="dodger_blue"

SHORE_PSHARE="9200"

nsplug meta_vehicle.moos targ_henry.moos -i -f WARP=$TIME_WARP \
       VNAME=$VNAME1            PSHARE_PORT="9201"              \
       MOOS_PORT="9001"         SHORE_PSHARE=$SHORE_PSHARE     \
       START_POS=$START_POS1    VCOLOR=$VCOLOR1

nsplug meta_vehicle.moos targ_gilda.moos -i -f WARP=$TIME_WARP \
       VNAME=$VNAME2            PSHARE_PORT="9202"              \
       MOOS_PORT="9002"         SHORE_PSHARE=$SHORE_PSHARE     \
       START_POS=$START_POS2    VCOLOR=$VCOLOR2

nsplug meta_shoreside.moos targ_shoreside.moos -i -f WARP=$TIME_WARP \
       MOOS_PORT="9000"         PSHARE_PORT=$SHORE_PSHARE  \
       VNAMES=$VNAME1:$VNAME2   DRESET=$DRESET             \
       POINTS=$POINTS

nsplug meta_vehicle.bhv targ_henry.bhv -i -f VNAME=$VNAME1    \
       START_POS=$START_POS1   LOITER_POS=$LOITER_POS1        \
       COLAVD=$COLAVD          OBAVD=$OBAVD 

nsplug meta_vehicle.bhv targ_gilda.bhv -i -f VNAME=$VNAME2    \
       START_POS=$START_POS2 LOITER_POS=$LOITER_POS2          \
       COLAVD=$COLAVD          OBAVD=$OBAVD 

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

#echo "Launching $VNAME2 MOOS Community. WARP is" $TIME_WARP
#pAntler targ_gilda.moos >& /dev/null &

uMAC --proc=pObstacleMgr --node=gilda targ_shoreside.moos

kill -- -$$
