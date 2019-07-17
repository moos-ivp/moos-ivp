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

#----------------------------------------------------------
#  Part 2: Check for and handle command-line arguments
#----------------------------------------------------------
for ARGI; do
    if [ "${ARGI}" = "--help" -o "${ARGI}" = "-h" ] ; then
	echo "launch.sh [SWITCHES] [time_warp]    " 
	echo "  --just_make, -j                   " 
	echo "  --help, -h                        " 
	exit 0;
    elif [ "${ARGI//[^0-9]/}" = "$ARGI" -a "$TIME_WARP" = 1 ]; then 
        TIME_WARP=$ARGI
    elif [ "${ARGI}" = "--just_make" -o "${ARGI}" = "-j" ] ; then
	JUST_MAKE="yes"
    else 
        echo "Bad arg:" $ARGI "Run with -h for help."
        echo "The launch.sh script is exiting with (1)."
        exit 1
    fi
done

#----------------------------------------------------------
#  Part 3: Create the .moos and .bhv files. 
#----------------------------------------------------------
VNAME1="henry"           # The first vehicle Community
VNAME2="gilda"           # The second vehicle Community
VGROUP1="red"
VGROUP2="blue"
VTYPE1="kayak"
VTYPE2="mokai"
START_POS1="0,0"         
START_POS2="180,0"        
LOITER_POS1="x=0,y=-75"
LOITER_POS2="x=125,y=-50"
SHORE_PSHARE="9200"

nsplug meta_vehicle.moos targ_henry.moos -i -f WARP=$TIME_WARP \
       VNAME=$VNAME1            BOT_PSHARE="9201"              \
       BOT_MOOSDB="9001"        SHORE_PSHARE=$SHORE_PSHARE     \
       START_POS=$START_POS1  

nsplug meta_vehicle.moos targ_gilda.moos -i -f WARP=$TIME_WARP \
       VNAME=$VNAME2            BOT_PSHARE="9202"              \
       BOT_MOOSDB="9002"        SHORE_PSHARE=$SHORE_PSHARE     \
       START_POS=$START_POS2  

nsplug meta_shoreside.moos targ_shoreside.moos -i -f WARP=$TIME_WARP \
       SHORE_PSHARE=$SHORE_PSHARE  SHORE_MOOSDB="9000"        \
       VNAMES=$VNAME1:$VNAME2

nsplug meta_vehicle.bhv targ_henry.bhv -i -f VNAME=$VNAME1    \
    START_POS=$START_POS1 LOITER_POS=$LOITER_POS1       

nsplug meta_vehicle.bhv targ_gilda.bhv -i -f VNAME=$VNAME2    \
    START_POS=$START_POS2 LOITER_POS=$LOITER_POS2       

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

uMAC targ_shoreside.moos
