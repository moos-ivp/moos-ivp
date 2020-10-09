#!/bin/bash -e
#----------------------------------------------------------
#  Script: launch.sh
#  Author: Michael Benjamin
#  LastEd: Apr 26th 2019
#----------------------------------------------------------
#  Part 1: Declare global var defaults
#----------------------------------------------------------
TIME_WARP=1
JUST_MAKE="no"
AUTO=""
LAUNCH_GUI="yes"
COLREGS="no"
MAX_ENCOUNTERS="1000"
MAX_TIME="-1"


#----------------------------------------------------------
#  Part 2: Check for and handle command-line arguments
#----------------------------------------------------------
for ARGI; do
    if [ "${ARGI}" = "--help" -o "${ARGI}" = "-h" ] ; then
	echo "launch.sh [SWITCHES] [time_warp]                      " 
	echo "  --help, -h                                          " 
	echo "  --just_make, -j                                     " 
        echo "  --auto, -a        Auto-launched. uMAC not used      "
        echo "  --nogui           Launch in headless mode           "
        echo "  --me=<amt>        Max encounters when uQueryDB used " 
        echo "  --mt=<secs>       Max time when uQueryDB used       " 
        echo "  --colregs, -c     Use COLREGS avoidance             "
	exit 0;
    elif [ "${ARGI//[^0-9]/}" = "$ARGI" -a "$TIME_WARP" = 1 ]; then 
        TIME_WARP=$ARGI
    elif [ "${ARGI}" = "--just_make" -o "${ARGI}" = "-j" ]; then
	JUST_MAKE="yes"
    elif [ "${ARGI}" = "--auto" -o "${ARGI}" = "-a" ]; then
        AUTO="yes"
    elif [ "${ARGI}" = "--nogui" -o "${ARGI}" = "-n" ]; then
	LAUNCH_GUI="no"
    elif [ "${ARGI:0:5}" = "--me=" ]; then
        MAX_ENCOUNTERS="${ARGI#--me=*}"
    elif [ "${ARGI:0:5}" = "--mt=" ]; then
        MAX_TIME="${ARGI#--mt=*}"
    elif [ "${ARGI}" = "--colregs" -o "${ARGI}" = "-c" ]; then
	COLREGS="yes"
    else 
        echo "launch.sh Bad arg:" $ARGI " Exiting with code: 1"
        exit 1
    fi
done

#----------------------------------------------------------
#  Part 3: Create the .moos and .bhv files. 
#----------------------------------------------------------
VNAME1="henry"           # The first vehicle Community
VNAME2="gilda"           # The second vehicle Community
START_POS1="0,0"         
START_POS2="180,0"        
LOITER_POS1="x=0,y=-75"
LOITER_POS2="x=125,y=-50"
SHORE_PSHARE="9200"

nsplug meta_vehicle.moos targ_henry.moos -i -f WARP=$TIME_WARP \
       VNAME=$VNAME1            PSHARE_PORT="9201"             \
       VPORT="9001"             SHORE_PSHARE=$SHORE_PSHARE     \
       START_POS=$START_POS1  

nsplug meta_vehicle.moos targ_gilda.moos -i -f WARP=$TIME_WARP \
       VNAME=$VNAME2            PSHARE_PORT="9202"             \
       VPORT="9002"             SHORE_PSHARE=$SHORE_PSHARE     \
       START_POS=$START_POS2  

nsplug meta_shoreside.moos targ_shoreside.moos -i -f WARP=$TIME_WARP \
       PSHARE_PORT="9200"       VNAMES=$VNAME1:$VNAME2         \
       LAUNCH_GUI=$LAUNCH_GUI   MAX_ENCOUNTERS=$MAX_ENCOUNTERS \
       MAX_TIME=$MAX_TIME

nsplug meta_vehicle.bhv targ_henry.bhv -i -f VNAME=$VNAME1    \
       START_POS=$START_POS1    LOITER_POS=$LOITER_POS1       \
       MIN_UTIL_CPA_DIST=10     MAX_UTIL_CPA_DIST=20          \
       COLREGS=$COLREGS

nsplug meta_vehicle.bhv targ_gilda.bhv -i -f VNAME=$VNAME2    \
       START_POS=$START_POS2    LOITER_POS=$LOITER_POS2       \
       MIN_UTIL_CPA_DIST=10     MAX_UTIL_CPA_DIST=20          \
       COLREGS=$COLREGS       

if [ ! -e targ_henry.moos ]; then echo "no targ_henry.moos"; exit; fi
if [ ! -e targ_henry.bhv  ]; then echo "no targ_henry.bhv "; exit; fi
if [ ! -e targ_gilda.moos ]; then echo "no targ_gilda.moos"; exit; fi
if [ ! -e targ_gilda.bhv  ]; then echo "no targ_gilda.bhv "; exit; fi
if [ ! -e targ_shoreside.moos ]; then echo "no targ_shoreside.moos"; exit; fi

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

echo "Launching $VNAME2 MOOS Community. WARP is" $TIME_WARP
pAntler targ_gilda.moos >& /dev/null &

#-------------------------------------------------------------- 
#  Part 5: Unless auto-launched, launch uMAC until mission quit          
#-------------------------------------------------------------- 
if [ "${AUTO}" = "" ]; then
    uMAC targ_shoreside.moos --node=shoreside --proc=uFldCollisionDetect
    kill -- -$$
fi
