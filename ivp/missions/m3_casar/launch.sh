#!/bin/bash -e
#----------------------------------------------------------
#  Script: launch.sh
#  Author: Michael Benjamin
#  LastEd: Apr 25th 2020
#----------------------------------------------------------
#  Part 1: Set Exit actions and declare global var defaults
#----------------------------------------------------------
TIME_WARP=1
JUST_MAKE="no"

#-------------------------------------------------------
#  Part 2: Check for and handle command-line arguments
#-------------------------------------------------------
for ARGI; do
    if [ "${ARGI}" = "--help" -o "${ARGI}" = "-h" ] ; then
	echo "launch.sh [SWITCHES] [time_warp] "
	echo "  --just_make, -j                " 
	echo "  --help, -h                     " 
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
VNAME1="henry"         # The first vehicle Community
VNAME2="gilda"         # The second vehicle Community
START_POS1="50,-50"    # Vehicle 1 Behavior configurations
START_POS2="80,-125"   # Vehicle 2 Behavior configurations
SHORE_LISTEN="9300"

nsplug meta_vehicle.moos targ_henry.moos -f WARP=$TIME_WARP \
   VNAME=$VNAME1      START_POS=$START_POS1                 \
   VPORT="9001"       SHARE_LISTEN="9301"                   \
   VTYPE="kayak"      SHORE_LISTEN=$SHORE_LISTEN            \
   KNOWS_CONTACTS=1

nsplug meta_vehicle.moos targ_gilda.moos -f WARP=$TIME_WARP \
   VNAME=$VNAME2      START_POS=$START_POS2                 \
   VPORT="9002"       SHARE_LISTEN="9302"                   \
   VTYPE="kayak"      SHORE_LISTEN=$SHORE_LISTEN

nsplug meta_shoreside.moos targ_shoreside.moos -f WARP=$TIME_WARP \
   SNAME="shoreside"  SHARE_LISTEN=$SHORE_LISTEN                  \
   SPORT="9000"     

nsplug meta_henry.bhv targ_henry.bhv -i -f VNAME=$VNAME1  \
    OVNAME=$VNAME2 START_POS=$START_POS2 

nsplug meta_gilda.bhv targ_gilda.bhv -i -f VNAME=$VNAME2  \
    OVNAME=$VNAME1 START_POS=$START_POS1 

if [ ${JUST_MAKE} = "yes" ] ; then
    exit 0
fi

#-------------------------------------------------------
#  Part 4: Launch the processes
#-------------------------------------------------------
echo "Launching $VNAME1 MOOS Community with WARP:" $TIME_WARP
pAntler targ_henry.moos >& /dev/null &
sleep 0.25
echo "Launching $VNAME2 MOOS Community with WARP:" $TIME_WARP
pAntler targ_gilda.moos >& /dev/null &
sleep 0.25
echo "Launching shoreside MOOS Community with WARP:"  $TIME_WARP
pAntler targ_shoreside.moos >& /dev/null &
echo "Done "

uMAC targ_shoreside.moos
kill -- -$$



