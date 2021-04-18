#!/bin/bash -e
#----------------------------------------------------------
#  Script: launch.sh
#  Author: Michael Benjamin
#  LastEd: Sep 20th 2020
#----------------------------------------------------------
#  Part 1: Set global var defaults
#----------------------------------------------------------
TIME_WARP=1
JUST_MAKE="no"
LAUNCH="manual"
VTYPE="kayak"
TLA="cnf"
TNUM="cnf_01"
MAX_TIME=600

VNAME1="abe"           # The first  vehicle Community
VNAME2="ben"           # The second vehicle Community
VCOLOR1="green"
VCOLOR2="dodger_blue"

#----------------------------------------------------------
#  Part 2: Check for and handle command-line arguments
#----------------------------------------------------------
for ARGI; do
    if [ "${ARGI}" = "--help" -o "${ARGI}" = "-h" ] ; then
	echo "launch.sh [SWITCHES] [time_warp]    " 
	echo "  --help, -h         Show this help message              " 
	echo "  --just_make, -j    Just create targ files, no launch   " 
        echo "  --auto, -a         Headless auto-launch. uMAC not used "
	echo "  --test=cnf_01      Config for variation_cnf_01.txt     " 
	echo "  -t01               Config for variation_cnf_01.txt     " 
	exit 0;
    elif [ "${ARGI//[^0-9]/}" = "$ARGI" -a "$TIME_WARP" = 1 ]; then 
        TIME_WARP=$ARGI
    elif [ "${ARGI}" = "--just_make" -o "${ARGI}" = "-j" ]; then
	JUST_MAKE="yes"
    elif [ "${ARGI}" = "--auto" -o "${ARGI}" = "-a" ]; then
        LAUNCH="auto"
    elif [ "${ARGI:0:2}" = "-t" ]; then
        IX="${ARGI#-t*}"
	TNUM=$TLA"_"$IX
    elif [ "${ARGI:0:7}" = "--test=" ]; then
        TNUM="${ARGI#--test=*}"
	
    else 
        echo "launch.sh Bad arg:" $ARGI " Exiting with code: 1"
        exit 1
    fi
done

#----------------------------------------------------------
#  Part 3: Create the .moos and .bhv files. 
#----------------------------------------------------------
nsplug meta_vehicle.moos targ_$VNAME1.moos -i -f WARP=$TIME_WARP \
       VNAME=$VNAME1            BOT_PSHARE="9201"                \
       BOT_MOOSDB="9001"        SHORE_PSHARE="9200"              \
       VCOLOR=$VCOLOR1          VTYPE=$VTYPE                     \
       TNUM=$TNUM               LAUNCH=$LAUNCH 

nsplug meta_vehicle.moos targ_$VNAME2.moos -i -f WARP=$TIME_WARP \
       VNAME=$VNAME2            BOT_PSHARE="9202"                \
       BOT_MOOSDB="9002"        SHORE_PSHARE="9200"              \
       VCOLOR=$VCOLOR2          VTYPE=$VTYPE                     \
       TNUM=$TNUM               LAUNCH=$LAUNCH 

nsplug meta_shoreside.moos targ_shoreside.moos -i -f WARP=$TIME_WARP \
       SHORE_MOOSDB="9000"      LAUNCH=$LAUNCH                \
       SHORE_PSHARE="9200"      MAX_TIME=$MAX_TIME

nsplug meta_vehicle.bhv targ_$VNAME1.bhv -i -f VNAME=$VNAME1  \
       TNUM=$TNUM

nsplug meta_vehicle.bhv targ_$VNAME2.bhv -i -f VNAME=$VNAME2  \
       TNUM=$TNUM


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

#-------------------------------------------------------------- 
#  Part 5: Unless auto-launched, launch uMAC until mission quit          
#-------------------------------------------------------------- 
if [ "${LAUNCH}" = "manual" ]; then
    uMAC targ_shoreside.moos --node=$VNAME2 --proc=pMissionEval
    kill -- -$$
fi

exit 0
