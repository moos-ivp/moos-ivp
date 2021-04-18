#!/bin/bash -e
#----------------------------------------------------------
#  Script: launch.sh
#  Author: Michael Benjamin
#  LastEd: May 17th 2019
#----------------------------------------------------------
#  Part 1: Set Exit actions and declare global var defaults
#----------------------------------------------------------
TIME_WARP=1
JUST_MAKE="no"

VNAME1="henry"           # The first vehicle Community
VNAME2="gilda"           # The second vehicle Community
START_POS1="150,-80,290"        
START_POS2="-20,-80,80"         
LOITER_POS1="x=-15,y=-70"
LOITER_POS2="x=150,y=-60"
SHORE_PSHARE="9200"

#----------------------------------------------------------
#  Part 2: Check for and handle command-line arguments
#----------------------------------------------------------
for ARGI; do
    if [ "${ARGI}" = "--help" -o "${ARGI}" = "-h" ] ; then
	echo "launch.sh [SWITCHES] [time_warp]    " 
	echo "  --help, -h          Show this help message               " 
        echo "  --just_make, -j     Just create targ files, no launch    "
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

#-------------------------------------------------------
#  Part 3: Create the .moos and .bhv files. 
#-------------------------------------------------------
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

kill -- -$$
