#!/bin/bash -e
#----------------------------------------------------------
#  Script: launch.sh
#  Author: Michael Benjamin
#  LastEd: Apr 25th 2020
#----------------------------------------------------------
#  Part 1: Set Exit actions and declare global var defaults
#----------------------------------------------------------
trap "kill -- -$$" EXIT SIGTERM SIGHUP SIGINT SIGKILL
TIME_WARP=1
JUST_MAKE="no"

#-------------------------------------------------------
#  Part 2: Check for and handle command-line arguments
#-------------------------------------------------------
for ARGI; do
    if [ "${ARGI}" = "--help" -o "${ARGI}" = "-h" ] ; then
	echo "launch.sh [SWITCHES] [time_warp]   " 
	echo "  --just_make, -j                  " 
	echo "  --help, -h                       " 
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
VNAME1="archie"
START_POS1="0,0"         # Vehicle 1 Behavior configurations
LOITER_POS1="x=-90,y=-130"

VNAME2="betty"
START_POS2="20,0"        # Vehicle 2 Behavior configurations
LOITER_POS2="x=-40,y=-340"

VNAME3="charlie"
START_POS3="40,0"        # Vehicle 3 Behavior configurations
LOITER_POS3="x=210,y=-390"

VNAME4="david"
START_POS4="60,0"        # Vehicle 4 Behavior configurations
LOITER_POS4="x=410,y=-200"

VNAME5="ernie"
START_POS5="80,0"        # Vehicle 5 Behavior configurations
LOITER_POS5="x=220,y=-20"

VNAME9="prey"
START_POS9="-180,-100"   # Vehicle 9 UUV configuration


nsplug meta_vehicle.moos targ_archie.moos -f WARP=$TIME_WARP  \
   VNAME=$VNAME1    START_POS=$START_POS1                     \
   VPORT="9001"     SHARE_LISTEN="9301"                       \
   VTYPE="kayak"

nsplug meta_vehicle.moos targ_betty.moos -f WARP=$TIME_WARP   \
   VNAME=$VNAME2    START_POS=$START_POS2                     \
   VPORT="9002"     SHARE_LISTEN="9302"                       \
   VTYPE="kayak"

nsplug meta_vehicle.moos targ_charlie.moos -f WARP=$TIME_WARP \
   VNAME=$VNAME3  START_POS=$START_POS3                       \
   VPORT="9003"     SHARE_LISTEN="9303"                       \
   VTYPE="kayak"

nsplug meta_vehicle.moos targ_david.moos -f WARP=$TIME_WARP   \
   VNAME=$VNAME4    START_POS=$START_POS4                     \
   VPORT="9004"     SHARE_LISTEN="9304"                       \
   VTYPE="kayak"

nsplug meta_vehicle.moos targ_ernie.moos -f WARP=$TIME_WARP   \
   VNAME=$VNAME5    START_POS=$START_POS5                     \
   VPORT="9005"     SHARE_LISTEN="9305"                       \
   VTYPE="kayak"

nsplug meta_uuv.moos targ_prey.moos -f WARP=$TIME_WARP        \
   VNAME=$VNAME9    START_POS=$START_POS9                     \
   VPORT="9009"     SHARE_LISTEN="9309"                       \
   VTYPE="uuv"

nsplug meta_shoreside.moos targ_shoreside.moos -f WARP=$TIME_WARP \
   SNAME="shoreside"  SHARE_LISTEN="9300"                     \
   SPORT="9000" 

#-------------------------------------------------------   
nsplug meta_vehicle.bhv targ_archie.bhv -f VNAME=$VNAME1  \
    START_POS=$START_POS1 LOITER_POS=$LOITER_POS1       

nsplug meta_vehicle.bhv targ_betty.bhv -f VNAME=$VNAME2   \
    START_POS=$START_POS2 LOITER_POS=$LOITER_POS2       

nsplug meta_vehicle.bhv targ_charlie.bhv -f VNAME=$VNAME3 \
    START_POS=$START_POS3 LOITER_POS=$LOITER_POS3       

nsplug meta_vehicle.bhv targ_david.bhv -f VNAME=$VNAME4   \
    START_POS=$START_POS4 LOITER_POS=$LOITER_POS4       

nsplug meta_vehicle.bhv targ_ernie.bhv -f VNAME=$VNAME5   \
    START_POS=$START_POS5 LOITER_POS=$LOITER_POS5       

nsplug meta_uuv.bhv targ_prey.bhv -f VNAME=$VNAME9        \
    START_POS=$START_POS9 LOITER_POS=$LOITER_POS5      

if [ ${JUST_MAKE} = "yes" ] ; then
    exit 0
fi

#-------------------------------------------------------
#  Part 4: Launch the processes
#-------------------------------------------------------

echo "Launching $VNAME1 MOOS Community with WARP:" $TIME_WARP
pAntler targ_archie.moos >& /dev/null &
sleep 0.25
echo "Launching $VNAME2 MOOS Community with WARP:" $TIME_WARP
pAntler targ_betty.moos >& /dev/null &
sleep 0.25
echo "Launching $VNAME3 MOOS Community with WARP:" $TIME_WARP
pAntler targ_charlie.moos >& /dev/null &
sleep 0.25
echo "Launching $VNAME4 MOOS Community with WARP:" $TIME_WARP
pAntler targ_david.moos >& /dev/null &
sleep 0.25
echo "Launching $VNAME5 MOOS Community with WARP:" $TIME_WARP
pAntler targ_ernie.moos >& /dev/null &
sleep 0.25
echo "Launching $SNAME MOOS Community with WARP:"  $TIME_WARP
pAntler targ_shoreside.moos >& /dev/null &
sleep 0.25
echo "Launching $VNAME9 MOOS Community with WARP:" $TIME_WARP
pAntler targ_prey.moos >& /dev/null &
echo "Done "

uMAC targ_shoreside.moos
