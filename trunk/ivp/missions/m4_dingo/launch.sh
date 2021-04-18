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
VARIATION="0"

#-------------------------------------------------------
#  Part 2: Check for and handle command-line arguments
#-------------------------------------------------------for ARGI; do
for ARGI; do
    if [ "${ARGI}" = "--help" -o "${ARGI}" = "-h" ] ; then
	echo "launch.sh [SWITCHES] [time_warp] "
	echo "  --just_make, -j                " 
	echo "  --help, -h                     " 
	exit 0;
    elif [ "${ARGI//[^0-9]/}" = "$ARGI" -a "$TIME_WARP" = 1 ]; then 
	TIME_WARP=$ARGI
    elif [ "${ARGI}" = "v1" ] ; then
	VARIATION="1"
    elif [ "${ARGI}" = "v2" ] ; then
	VARIATION="2"
    elif [ "${ARGI}" = "v3" ] ; then
	VARIATION="3"
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
VNAME1="ella"           # The first  vehicle Community
VNAME2="frank"           # The second vehicle Community
VNAME3="gilda"           # The third  vehicle Community
VNAME4="henry"           # The fourth vehicle Community
START_POS1="0,0"         
START_POS2="20,0"        
START_POS3="40,0"         
START_POS4="60,50"        
LOITER_POS1="x=0,y=-75"
LOITER_POS2="x=125,y=-50"
LOITER_POS3="x=0,y=-175"
LOITER_POS4="x=125,y=-150"
SHORE_LISTEN="9300"

nsplug meta_vehicle.moos targ_ella.moos -f WARP=$TIME_WARP \
    VNAME=$VNAME1          SHARE_LISTEN="9301"              \
    VPORT="9001"           SHORE_LISTEN=$SHORE_LISTEN       \
    START_POS=$START_POS1  VARIATION=$VARIATION             

nsplug meta_vehicle.moos targ_frank.moos -f WARP=$TIME_WARP \
    VNAME=$VNAME2          SHARE_LISTEN="9302"              \
    VPORT="9002"           SHORE_LISTEN=$SHORE_LISTEN       \
    START_POS=$START_POS2  VARIATION=$VARIATION             


nsplug meta_vehicle.moos targ_gilda.moos -f WARP=$TIME_WARP \
    VNAME=$VNAME3          SHARE_LISTEN="9303"              \
    VPORT="9003"           SHORE_LISTEN=$SHORE_LISTEN       \
    START_POS=$START_POS3  VARIATION=$VARIATION             

nsplug meta_vehicle.moos targ_henry.moos -f WARP=$TIME_WARP \
    VNAME=$VNAME4          SHARE_LISTEN="9304"              \
    VPORT="9004"           SHORE_LISTEN=$SHORE_LISTEN       \
    START_POS=$START_POS4  VARIATION=$VARIATION             

nsplug meta_shoreside.moos targ_shoreside.moos -f WARP=$TIME_WARP \
    SNAME="shoreside"  SHARE_LISTEN=$SHORE_LISTEN                 \
    SPORT="9000"       VARIATION=$VARIATION         

nsplug meta_vehicle.bhv targ_ella.bhv -f VNAME=$VNAME1     \
    START_POS=$START_POS1 LOITER_POS=$LOITER_POS1       

nsplug meta_vehicle.bhv targ_frank.bhv -f VNAME=$VNAME2     \
    START_POS=$START_POS2 LOITER_POS=$LOITER_POS2       

nsplug meta_vehicle.bhv targ_gilda.bhv -f VNAME=$VNAME3     \
    START_POS=$START_POS3 LOITER_POS=$LOITER_POS3       

nsplug meta_vehicle.bhv targ_henry.bhv -f VNAME=$VNAME4     \
    START_POS=$START_POS4 LOITER_POS=$LOITER_POS4       

if [ ${JUST_MAKE} = "yes" ] ; then
    exit 0
fi

#-------------------------------------------------------
#  Part 4: Launch the processes
#-------------------------------------------------------
echo "Launching $SNAME MOOS Community with WARP:"  $TIME_WARP
pAntler targ_shoreside.moos >& /dev/null &
sleep 0.25

echo "Launching $VNAME1 MOOS Community with WARP:" $TIME_WARP
pAntler targ_ella.moos >& /dev/null &
sleep 0.25
echo "Launching $VNAME2 MOOS Community with WARP:" $TIME_WARP
pAntler targ_frank.moos >& /dev/null &

echo "Launching $VNAME3 MOOS Community with WARP:" $TIME_WARP
pAntler targ_gilda.moos >& /dev/null &
sleep 0.25
echo "Launching $VNAME4 MOOS Community with WARP:" $TIME_WARP
pAntler targ_henry.moos >& /dev/null &

echo "Done."

uMAC targ_shoreside.moos


