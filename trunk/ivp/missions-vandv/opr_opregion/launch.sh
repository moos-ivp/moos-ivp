#!/bin/bash -e
#----------------------------------------------------------
#  Script: launch.sh
#  Author: Michael Benjamin
#  LastEd: Sep 24th 2020
#----------------------------------------------------------
#  Part 1: Set global var defaults
#----------------------------------------------------------
TIME_WARP=1
LAUNCH="manual"
JUST_MAKE="no"
TNUM="opr_01"

#----------------------------------------------------------
#  Part 2: Check for and handle command-line arguments
#----------------------------------------------------------
for ARGI; do
    if [ "${ARGI}" = "--help" -o "${ARGI}" = "-h" ] ; then
	echo "launch.sh [SWITCHES] [time_warp]                         "
	echo "  --help, -h                                             " 
	echo "  --just_make, -j    Just create targ files, no launch   " 
        echo "  --auto, -a         Headless auto-launch. uMAC not used "
	echo "  --test=opr_01      Config for variation_opr_01.txt     " 
	exit 0;
    elif [ "${ARGI//[^0-9]/}" = "$ARGI" -a "$TIME_WARP" = 1 ]; then 
        TIME_WARP=$ARGI
    elif [ "${ARGI}" = "--just_make" -o "${ARGI}" = "-j" ]; then
	JUST_MAKE="yes"
    elif [ "${ARGI}" = "--auto" -o "${ARGI}" = "-a" ]; then
        LAUNCH="auto"
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
nsplug meta_alpha.moos targ_alpha.moos -i -f WARP=$TIME_WARP \
       TNUM=$TNUM      LAUNCH=$LAUNCH   MAX_TIME=600

nsplug meta_alpha.bhv targ_alpha.bhv -i -f TNUM=$TNUM


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
if [ "${LAUNCH}" = "manual" ]; then
    uMAC targ_alpha.moos --node=alpha --proc=pMissionEval
    kill -- -$$
fi

exit 0
