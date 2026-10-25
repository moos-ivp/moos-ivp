#!/bin/bash -e
#----------------------------------------------------------
#  Script: launch.sh
#  Author: Michael Benjamin
#  LastEd: Jan 1st 2023
#----------------------------------------------------------
#  Part 1: Set global var defaults
#----------------------------------------------------------
TIME_WARP=1
MU_A=175
MU_B=50
SIG_A=4
SIG_B=6

#----------------------------------------------------------
#  Part 2: Check for and handle command-line arguments
#----------------------------------------------------------
for ARGI; do
    if [ "${ARGI}" = "--help" -o "${ARGI}" = "-h" ]; then
	echo "launch.sh [SWITCHES] [time_warp]    " 
	echo "  --help, -h         Show this help message            " 
	echo "  --just_make, -j    Just create targ files, no launch " 
	echo "                                                       " 
	echo "  --siga=val         Set sigma A [4]    " 
	echo "  --siga=val         Set sigma B [6]    " 
	echo "                                        " 
	echo "  --mua=val          Set Mu A [175]     " 
	echo "  --mub=val          Set Mu B [50]      " 
	exit 0;
    elif [ "${ARGI//[^0-9]/}" = "$ARGI" -a "$TIME_WARP" = 1 ]; then 
        TIME_WARP=$ARGI
    elif [ "${ARGI:0:6}" = "--mua=" ]; then
        MU_A="${ARGI#--mua=*}"
    elif [ "${ARGI:0:6}" = "--mub=" ]; then
        MU_B="${ARGI#--mub=*}"
    elif [ "${ARGI:0:7}" = "--siga=" ]; then
        SIG_A="${ARGI#--siga=*}"
    elif [ "${ARGI:0:7}" = "--sigb=" ]; then
        SIG_B="${ARGI#--sigb=*}"
    else 
        echo "launch.sh Bad arg:" $ARGI " Exiting with code: 1"
        exit 1
    fi
done

#-------------------------------------------------------
#  Part 3: Create the .moos file
#-------------------------------------------------------
nsplug meta_plotting.moos targ_plotting.moos -i -f WARP=$TIME_WARP \
       MU_A=$MU_A           SIG_A=$SIG_A    \
       MU_B=$MU_B           SIG_B=$SIG_B 

#-------------------------------------------------------
#  Part 4: Launch
#-------------------------------------------------------
echo "Launching MOOS Community. WARP is" $TIME_WARP
pAntler targ_plotting.moos

kill -- -$$
