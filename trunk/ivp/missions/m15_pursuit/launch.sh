#!/bin/bash -e
#----------------------------------------------------------
#  Script: launch.sh
#  Author: Michael Benjamin
#  LastEd: Sep 3rd 2020
#----------------------------------------------------------
#  Part 1: Set Exit actions and declare global var defaults
#----------------------------------------------------------
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
        echo "launch.sh Bad arg:" $ARGI " Exiting with code: 1"
        exit 1
    fi
done

#----------------------------------------------------------
#  Part 3: Create the .moos and .bhv files. 
#----------------------------------------------------------
VNAME1="chaser"           # The first vehicle Community
VNAME2="evader"           # The second vehicle Community
VTYPE1="kayak"
VTYPE2="kayak"
GROUP1="chaser_grp"
GROUP2="evader_grp"
START_POS1="0,0"         
START_POS2="180,0"        
SHORE_PSHARE="9200"

nsplug meta_vehicle.moos targ_$VNAME1.moos -i -f WARP=$TIME_WARP \
       VNAME=$VNAME1            BOT_PSHARE="9201"              \
       BOT_MOOSDB="9001"        SHORE_PSHARE=$SHORE_PSHARE     \
       START_POS=$START_POS1    GROUP=$GROUP1

nsplug meta_vehicle.moos targ_$VNAME2.moos -i -f WARP=$TIME_WARP \
       VNAME=$VNAME2            BOT_PSHARE="9202"              \
       BOT_MOOSDB="9002"        SHORE_PSHARE=$SHORE_PSHARE     \
       START_POS=$START_POS2    GROUP=$GROUP1

nsplug meta_shoreside.moos targ_shoreside.moos -i -f WARP=$TIME_WARP \
       SHORE_PSHARE=$SHORE_PSHARE  SHORE_MOOSDB="9000"        \
       VNAMES=$VNAME1:$VNAME2

nsplug meta_$VNAME1.bhv targ_$VNAME1.bhv -i -f VNAME=$VNAME1    \
    START_POS=$START_POS1 

nsplug meta_$VNAME2.bhv targ_$VNAME2.bhv -i -f VNAME=$VNAME2    \
    START_POS=$START_POS2 

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
pAntler targ_$VNAME1.moos >& /dev/null &

echo "Launching $VNAME2 MOOS Community. WARP is" $TIME_WARP
pAntler targ_$VNAME2.moos >& /dev/null &

uMAC targ_shoreside.moos

kill -- -$$
