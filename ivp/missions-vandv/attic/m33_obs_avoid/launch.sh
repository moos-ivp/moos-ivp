#!/bin/bash -e
#----------------------------------------------------------
#  Script: launch.sh
#  Author: Michael Benjamin
#  LastEd: May 17th 2019
#----------------------------------------------------------
#  Part 1: Set Exit actions and declare global var defaults
#----------------------------------------------------------
trap "kill -- -$$" EXIT SIGTERM SIGHUP SIGINT SIGKILL
TIME_WARP=1
JUST_MAKE="no"
LAUNCH_GUI="yes"

#----------------------------------------------------------
#  Part 2: Check for and handle command-line arguments
#----------------------------------------------------------
for ARGI; do
    if [ "${ARGI}" = "--help" -o "${ARGI}" = "-h" ] ; then
	echo "launch.sh [SWITCHES] [time_warp]    " 
	echo "  --just_make, -j                   " 
	echo "  --nogui, -n                       " 
	echo "  --help, -h                        " 
	exit 0;
    elif [ "${ARGI//[^0-9]/}" = "$ARGI" -a "$TIME_WARP" = 1 ]; then 
        TIME_WARP=$ARGI
    elif [ "${ARGI}" = "--just_make" -o "${ARGI}" = "-j" ] ; then
	JUST_MAKE="yes"
    elif [ "${ARGI}" = "--nogui" -o "${ARGI}" = "-n" ] ; then
        LAUNCH_GUI="no"
    else 
        echo "Bad arg:" $ARGI "Run with -h for help."
        echo "The launch.sh script is exiting with (1)."
        exit 1
    fi
done

#-------------------------------------------------------
#  Part 3: Create the .moos and .bhv files. 
#-------------------------------------------------------
VNAME="abe"
START_POS="0,-100,80"         
LOITER_POS="x=-10,y=-45"
SHORE_PSHARE="9200"

nsplug meta_vehicle.moos targ_abe.moos -i -f WARP=$TIME_WARP   \
       VNAME=$VNAME             BOT_PSHARE="9201"              \
       BOT_MOOSDB="9001"        SHORE_PSHARE=$SHORE_PSHARE     \
       START_POS=$START_POS

nsplug meta_shoreside.moos targ_shoreside.moos -i -f WARP=$TIME_WARP \
       SHORE_PSHARE=$SHORE_PSHARE  SHORE_MOOSDB="9000"     \
       LAUNCH_GUI=$LAUNCH_GUI

nsplug meta_vehicle.bhv targ_abe.bhv -i -f VNAME=$VNAME    \
    START_POS=$START_POS LOITER_POS=$LOITER_POS 

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
pAntler targ_abe.moos >& /dev/null &

uMAC targ_shoreside.moos
