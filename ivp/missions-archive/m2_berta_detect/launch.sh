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
AUTO=""
LAUNCH_GUI="yes"

MAX_ENCOUNTERS="1000"
MAX_TIME="-1"
COLAVD="colregs"
MISSION_TAG=""
PATIENCE="50"

#-------------------------------------------------------
#  Part 2: Check for and handle command-line arguments
#-------------------------------------------------------
for ARGI; do
    echo "Arg:["$ARGI"]"
    if [ "${ARGI}" = "--help" -o "${ARGI}" = "-h" ]; then
	echo "launch.sh [SWITCHES] [time_warp] "
	echo "  --help, -h          Show this help message               " 
        echo "  --just_make, -j     Just create targ files, no launch    "
        echo "  --auto, -a          Auto-launched. uMAC not used         "
        echo "  --nogui, -n         Launch in headless mode              " 
        echo "  --me=<amt>          Max encounters when uQueryDB used    " 
        echo "  --mt=<secs>         Max time when uQueryDB used          " 
        echo "  --avd=cpa           Basic CPA coll avoidance, no COLREGS " 
        echo "  --avd=off           No Collision avoidance at all        " 
        echo "  --pat=<N=1..99>     Set Loiter patience to N (default 50)" 
        echo "  --tag=<tag>         Post MISSION_TAG=<tag> in shoreside  " 
	exit 0;
    elif [ "${ARGI//[^0-9]/}" = "$ARGI" -a "$TIME_WARP" = 1 ]; then 
        TIME_WARP=$ARGI
    elif [ "${ARGI}" = "--just_make" -o "${ARGI}" = "-j" ]; then
	JUST_MAKE="yes"
    elif [ "${ARGI}" = "--auto" -o "${ARGI}" = "-a" ]; then
        AUTO="yes"
    elif [ "${ARGI}" = "--nogui" -o "${ARGI}" = "-n" ]; then
	LAUNCH_GUI="no"
    elif [ "${ARGI:0:6}" = "--tag=" ]; then
        MISSION_TAG="${ARGI#--tag=*}"
    elif [ "${ARGI:0:5}" = "--me=" ]; then
        MAX_ENCOUNTERS="${ARGI#--me=*}"
    elif [ "${ARGI:0:5}" = "--mt=" ]; then
        MAX_TIME="${ARGI#--mt=*}"
    elif [ "${ARGI}" = "--avd=cpa" ]; then
	COLAVD="cpa"
    elif [ "${ARGI}" = "--avd=off" ]; then
	COLAVD="off"
    elif [ "${ARGI:0:6}" = "--pat=" ]; then
        PATIENCE="${ARGI#--pat=*}"
    else 
        echo "launch.sh Bad arg:" $ARGI " Exiting with code: 1"
        exit 1
    fi
done

#-------------------------------------------------------
#  Part 3: Create the .moos and .bhv files. 
#-------------------------------------------------------
VNAME1="abe"           # The first vehicle Community
VNAME2="ben"           # The second vehicle Community
VNAME3="cal"           # The third vehicle Community
VNAME4="deb"           # The fourth vehicle Community
START_POS1="0,0"         
START_POS2="80,0"        
START_POS3="40,20"        
START_POS4="60,20"        
LOITER_POS1="x=0,y=-75"
LOITER_POS2="x=125,y=-50"
SHORE_LISTEN="9300"

nsplug meta_vehicle.moos targ_abe.moos -f WARP=$TIME_WARP \
       VNAME=$VNAME1          SHARE_LISTEN="9301"         \
       VPORT="9001"           SHORE_LISTEN=$SHORE_LISTEN  \
       START_POS=$START_POS1  

nsplug meta_vehicle.moos targ_ben.moos -f WARP=$TIME_WARP \
       VNAME=$VNAME2          SHARE_LISTEN="9302"         \
       VPORT="9002"           SHORE_LISTEN=$SHORE_LISTEN  \
       START_POS=$START_POS2  

nsplug meta_vehicle.moos targ_cal.moos -f WARP=$TIME_WARP \
       VNAME=$VNAME3          SHARE_LISTEN="9303"         \
       VPORT="9003"           SHORE_LISTEN=$SHORE_LISTEN  \
       START_POS=$START_POS3  

nsplug meta_vehicle.moos targ_deb.moos -f WARP=$TIME_WARP \
       VNAME=$VNAME4          SHARE_LISTEN="9304"         \
       VPORT="9004"           SHORE_LISTEN=$SHORE_LISTEN  \
       START_POS=$START_POS4  

nsplug meta_shoreside.moos targ_shoreside.moos -f WARP=$TIME_WARP \
       SNAME="shoreside"      SHARE_LISTEN=$SHORE_LISTEN          \
       SPORT="9000"           MISSION_TAG=$MISSION_TAG            \
       VNAME1=$VNAME1         LAUNCH_GUI=$LAUNCH_GUI              \
       VNAME2=$VNAME2         MAX_ENCOUNTERS=$MAX_ENCOUNTERS      \
       MAX_TIME=$MAX_TIME
	  

nsplug meta_vehicle.bhv targ_abe.bhv -f VNAME=$VNAME1     \
       START_POS=$START_POS1     COLAVD=$COLAVD           \
       LOITER_POS=$LOITER_POS1   MISSION_ROLE="SIDE"      \
       PATIENCE=$PATIENCE

nsplug meta_vehicle.bhv targ_ben.bhv -f VNAME=$VNAME2     \
       START_POS=$START_POS2     COLAVD=$COLAVD           \
       LOITER_POS=$LOITER_POS2   MISSION_ROLE="SIDE"      \
       PATIENCE=$PATIENCE

nsplug meta_vehicle.bhv targ_cal.bhv -f VNAME=$VNAME3     \
       START_POS=$START_POS3     COLAVD=$COLAVD           \
       MISSION_ROLE="MIDDLE"     PATIENCE=$PATIENCE

nsplug meta_vehicle.bhv targ_deb.bhv -f VNAME=$VNAME4     \
       START_POS=$START_POS4     COLAVD=$COLAVD           \
       MISSION_ROLE="MIDDLE"     PATIENCE=$PATIENCE

if [ ! -e targ_abe.moos ]; then echo "no targ_abe.moos"; exit; fi
if [ ! -e targ_abe.bhv  ]; then echo "no targ_abe.bhv "; exit; fi
if [ ! -e targ_ben.moos ]; then echo "no targ_ben.moos"; exit; fi
if [ ! -e targ_ben.bhv  ]; then echo "no targ_ben.bhv "; exit; fi
if [ ! -e targ_cal.moos ]; then echo "no targ_cal.moos"; exit; fi
if [ ! -e targ_cal.bhv  ]; then echo "no targ_cal.bhv "; exit; fi
if [ ! -e targ_deb.moos ]; then echo "no targ_deb.moos"; exit; fi
if [ ! -e targ_deb.bhv  ]; then echo "no targ_deb.bhv "; exit; fi
if [ ! -e targ_shoreside.moos ]; then echo "no targ_shoreside.moos";  exit; fi

if [ ${JUST_MAKE} = "yes" ] ; then
    exit 0
fi

#-------------------------------------------------------
#  Part 4: Launch the processes
#-------------------------------------------------------
echo "Launching $SNAME MOOS Community with WARP:" $TIME_WARP
pAntler targ_shoreside.moos >& /dev/null &
echo "Launching $VNAME1 MOOS Community with WARP:" $TIME_WARP
pAntler targ_abe.moos >& /dev/null &
echo "Launching $VNAME2 MOOS Community with WARP:" $TIME_WARP
pAntler targ_ben.moos >& /dev/null &
echo "Launching $VNAME3 MOOS Community with WARP:" $TIME_WARP
pAntler targ_cal.moos >& /dev/null &
echo "Launching $VNAME4 MOOS Community with WARP:" $TIME_WARP
pAntler targ_deb.moos >& /dev/null &
echo "Done "

#-------------------------------------------------------------- 
#  Part 5: Unless auto-launched, launch uMAC until mission quit          
#-------------------------------------------------------------- 

if [ "${AUTO}" = "yes" ]; then
    exit 0
fi

uMAC targ_shoreside.moos --node=shoreside --proc=uFldCollisionDetect
kill -- -$$
