#!/bin/bash -e
#----------------------------------------------------------
#  Script: launch.sh
#  Author: Michael Benjamin
#  LastEd: Dec 11th 2021
#----------------------------------------------------------
#  Part 1: Set global var defaults
#----------------------------------------------------------
TIME_WARP=1
JUST_MAKE="no"
LAUNCH_GUI="yes"
MAX_TIME="600"
SILENT="no"
SPD=3

#----------------------------------------------------------
#  Part 2: Check for and handle command-line arguments
#----------------------------------------------------------
for ARGI; do
    if [ "${ARGI}" = "--help" -o "${ARGI}" = "-h" ]; then
	echo "launch.sh [SWITCHES] [time_warp]                         "
	echo "  --help, -h                                             " 
	echo "  --just_make, -j    Just create targ files, no launch   " 
	echo "  --max_time=N       Max time headless mission will run  " 
        echo "  --nogui, -n        Headless mission, no pMarineViewer  "
        echo "  --auto, -a         Headless mission, no pMarineViewer  "
        echo "  --spd=N            Vehicle speed [0,4]                 "
	exit 0;
    elif [ "${ARGI//[^0-9]/}" = "$ARGI" -a "$TIME_WARP" = 1 ]; then 
        TIME_WARP=$ARGI
    elif [ "${ARGI}" = "--just_make" -o "${ARGI}" = "-j" ]; then
	JUST_MAKE="yes"
    elif [ "${ARGI:0:6}" = "--spd=" ]; then
        SPD="${ARGI#--spd=*}"
    elif [ "${ARGI:0:11}" = "--max_time=" ]; then
        MAX_TIME="${ARGI#--max_time=*}"
    elif [ "${ARGI}" = "--nogui" -o "${ARGI}" = "-n" ]; then
        LAUNCH_GUI="no"
    elif [ "${ARGI}" = "--auto" -o "${}"]; then
        LAUNCH_GUI="no"

    else
        echo "launch.sh Bad arg:" $ARGI " Exiting with code: 1"
        exit 1
    fi
done

#----------------------------------------------------------
#  Part 3: Create the .moos and .bhv files. 
#----------------------------------------------------------
nsplug meta_alpha.moos targ_alpha.moos -i -f WARP=$TIME_WARP \
       MAX_TIME=$MAX_TIME       LAUNCH_GUI=$LAUNCH_GUI   
       
nsplug meta_alpha.bhv targ_alpha.bhv -i -f SPD=$SPD

if [ ${JUST_MAKE} = "yes" ]; then
    echo "Files assembled; nothing launched; exiting per request."
    exit 0
fi

#----------------------------------------------------------
#  Part 4: Launch the mission processes
#----------------------------------------------------------
echo "Launching alpha MOOS Community. WARP is" $TIME_WARP
pAntler targ_alpha.moos >& /dev/null &

#-------------------------------------------------------------- 
#  Part 5: Unless auto-launched, launch uMAC until mission quit 
#-------------------------------------------------------------- 
if [ "${LAUNCH_GUI}" = "yes" ]; then
    uMAC targ_alpha.moos --node=alpha --proc=pMissionEval
    kill -- -$$
fi

exit 0
