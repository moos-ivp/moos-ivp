#!/bin/bash -e
#----------------------------------------------------------
#  Script: launch.sh
#  Author: Michael Benjamin
#  LastEd: May 17th 2019
#----------------------------------------------------------
#  Part 1: Set global var defaults
#----------------------------------------------------------
trap "kill -- -$$" EXIT SIGTERM SIGHUP SIGINT SIGKILL
TIME_WARP=1
JUST_MAKE="no"

VNAME1="abe"           # The first  vehicle Community
VNAME2="ben"           # The second vehicle Community
VNAME3="cal"           # The third  vehicle Community
VTYPE="kayak"
START_POS1="-30,-70,90"        
START_POS2="170,-80,270"         
GOTO_POS1="x=190,y=-80"
GOTO_POS2="x=-45,y=-80"
SHORE_PSHARE="9200"
POD1=120                # pwt_outer_dist
POD2=120
MIN_CPA1=6             # min_util_cpa
MIN_CPA2=6
MAX_CPA1=30             # min_util_cpa
MAX_CPA2=30
VCOLOR1="green"
VCOLOR2="dodger_blue"
VCOLOR3="dodger_blue"

#----------------------------------------------------------
#  Part 2: Check for and handle command-line arguments
#----------------------------------------------------------
for ARGI; do
    if [ "${ARGI}" = "--help" -o "${ARGI}" = "-h" ] ; then
	echo "launch.sh [SWITCHES] [time_warp]    " 
	echo "  --help, -h           Show this help message            " 
	echo "  --just_make, -j      Just create targ files, no launch " 
	echo "  --fast, -f           Init positions for fast encounter " 
	exit 0;
    elif [ "${ARGI//[^0-9]/}" = "$ARGI" -a "$TIME_WARP" = 1 ]; then 
        TIME_WARP=$ARGI
    elif [ "${ARGI}" = "--just_make" -o "${ARGI}" = "-j" ] ; then
	JUST_MAKE="yes"
    elif [ "${ARGI}" = "--pod" -o "${ARGI}" = "-p" ] ; then
	POD1=30
	POD2=80
    elif [ "${ARGI}" = "--min_cpa" -o "${ARGI}" = "-c" ] ; then
	MIN_CPA1=5
	MAX_CPA1=15
	MIN_CPA2=10
	MAX_CPA2=20
    else 
        echo "launch.sh Bad arg:" $ARGI " Exiting with code: 1"
        exit 1
    fi
done

#----------------------------------------------------------
#  Part 3: Create the .moos and .bhv files. 
#----------------------------------------------------------
nsplug meta_vehicle.moos targ_abe.moos -i -f WARP=$TIME_WARP \
       VNAME=$VNAME1            BOT_PSHARE="9201"            \
       BOT_MOOSDB="9001"        SHORE_PSHARE=$SHORE_PSHARE   \
       VTYPE=$VTYPE             START_POS=$START_POS1        \
       VCOLOR=$VCOLOR1

nsplug meta_vehicle.moos targ_ben.moos -i -f WARP=$TIME_WARP \
       VNAME=$VNAME2            BOT_PSHARE="9202"            \
       BOT_MOOSDB="9002"        SHORE_PSHARE=$SHORE_PSHARE   \
       VTYPE=$VTYPE             START_POS=$START_POS2        \
       VCOLOR=$VCOLOR2

nsplug meta_vehicle.moos targ_cal.moos -i -f WARP=$TIME_WARP \
       VNAME=$VNAME3            BOT_PSHARE="9203"            \
       BOT_MOOSDB="9003"        SHORE_PSHARE=$SHORE_PSHARE   \
       VTYPE=$VTYPE             START_POS=$START_POS2        \
       VCOLOR=$VCOLOR3

nsplug meta_shoreside.moos targ_shoreside.moos -i -f WARP=$TIME_WARP \
       SHORE_PSHARE=$SHORE_PSHARE  SHORE_MOOSDB="9000"       \
       VNAMES=$VNAME1:$VNAME2

nsplug meta_vehicle.bhv targ_abe.bhv -i -f VNAME=$VNAME1     \
       START_POS=$START_POS1 GOTO_POS=$GOTO_POS1 POD=50      \
       MIN_CPA=6 MAX_CPA=20
      
nsplug meta_vehicle.bhv targ_ben.bhv -i -f VNAME=$VNAME2     \
       START_POS=$START_POS2 GOTO_POS=$GOTO_POS2 POD=$POD1   \
       MIN_CPA=$MIN_CPA1 MAX_CPA=$MAX_CPA1

nsplug meta_vehicle.bhv targ_cal.bhv -i -f VNAME=$VNAME3     \
       START_POS=$START_POS2 GOTO_POS=$GOTO_POS2 POD=$POD2   \
       MIN_CPA=$MIN_CPA2   MAX_CPA=$MAX_CPA2

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

echo "Launching $VNAME2 MOOS Community. WARP is" $TIME_WARP
pAntler targ_ben.moos >& foostuff &

#echo "Launching $VNAME3 MOOS Community. WARP is" $TIME_WARP
#pAntler targ_cal.moos >& /dev/null &

uMAC targ_shoreside.moos

kill -- -$$
