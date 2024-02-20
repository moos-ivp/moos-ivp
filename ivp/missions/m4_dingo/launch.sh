#!/bin/bash -e
#----------------------------------------------------------
#  Script: launch.sh
#  Author: Michael Benjamin
#  LastEd: Feb 20th 2024
#----------------------------------------------------------
#  Part 1: Set Exit actions and declare global var defaults
#----------------------------------------------------------
TIME_WARP=1
JUST_MAKE="no"

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
VNAME1="abe"           
VNAME2="ben"           
VNAME3="cal"           
VNAME4="deb"           
START_POS1="0,0"         
START_POS2="20,0"        
START_POS3="40,0"         
START_POS4="60,50"        
LOITER_POS1="x=0,y=-75"
LOITER_POS2="x=125,y=-50"
LOITER_POS3="x=0,y=-125"
LOITER_POS4="x=125,y=-120"
SHORE_PSHARE="9200"

nsplug meta_vehicle.moos targ_$VNAME1.moos -f WARP=$TIME_WARP \
    VNAME=$VNAME1          PSHARE_PORT="9201"              \
    VPORT="9001"           SHORE_PSHARE=$SHORE_PSHARE      \
    START_POS=$START_POS1 

nsplug meta_vehicle.moos targ_$VNAME2.moos -f WARP=$TIME_WARP \
    VNAME=$VNAME2          PSHARE_PORT="9202"              \
    VPORT="9002"           SHORE_PSHARE=$SHORE_PSHARE      \
    START_POS=$START_POS2 


nsplug meta_vehicle.moos targ_$VNAME3.moos -f WARP=$TIME_WARP \
    VNAME=$VNAME3          PSHARE_PORT="9203"              \
    VPORT="9003"           SHORE_PSHARE=$SHORE_PSHARE      \
    START_POS=$START_POS3 

nsplug meta_vehicle.moos targ_$VNAME4.moos -f WARP=$TIME_WARP \
    VNAME=$VNAME4          PSHARE_PORT="9204"              \
    VPORT="9004"           SHORE_PSHARE=$SHORE_PSHARE      \
    START_POS=$START_POS4        

nsplug meta_shoreside.moos targ_shoreside.moos -f WARP=$TIME_WARP \
    MOOS_PORT="9000"       PSHARE_PORT=$SHORE_PSHARE
    

nsplug meta_vehicle.bhv targ_$VNAME1.bhv -f VNAME=$VNAME1     \
    START_POS=$START_POS1 LOITER_POS=$LOITER_POS1       

nsplug meta_vehicle.bhv targ_$VNAME2.bhv -f VNAME=$VNAME2     \
    START_POS=$START_POS2 LOITER_POS=$LOITER_POS2       

nsplug meta_vehicle.bhv targ_$VNAME3.bhv -f VNAME=$VNAME3     \
    START_POS=$START_POS3 LOITER_POS=$LOITER_POS3       

nsplug meta_vehicle.bhv targ_$VNAME4.bhv -f VNAME=$VNAME4     \
    START_POS=$START_POS4 LOITER_POS=$LOITER_POS4       

if [ ${JUST_MAKE} = "yes" ]; then
    exit 0
fi

#-------------------------------------------------------
#  Part 4: Launch the processes
#-------------------------------------------------------
echo "Launching Shoreside MOOS Community with WARP:"  $TIME_WARP
pAntler targ_shoreside.moos >& /dev/null &
sleep 0.25

echo "Launching $VNAME1 MOOS Community with WARP:" $TIME_WARP
pAntler targ_$VNAME1.moos >& /dev/null &
sleep 0.25
echo "Launching $VNAME2 MOOS Community with WARP:" $TIME_WARP
pAntler targ_$VNAME2.moos >& /dev/null &

echo "Launching $VNAME3 MOOS Community with WARP:" $TIME_WARP
pAntler targ_$VNAME3.moos >& /dev/null &
sleep 0.25
echo "Launching $VNAME4 MOOS Community with WARP:" $TIME_WARP
pAntler targ_$VNAME4.moos >& /dev/null &

echo "Done."

uMAC targ_shoreside.moos

kill -- -$$

