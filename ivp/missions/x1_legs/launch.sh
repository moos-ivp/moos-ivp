#!/bin/bash 
#----------------------------------------------------------
#  Script: launch.sh
#  Author: Michael Benjamin
#  LastEd: May 20th 2019
#----------------------------------------------------------
#  Part 1: Set Exit actions and declare global var defaults
#----------------------------------------------------------

TIME_WARP=1
COMMUNITY="alpha"
CLL_KTM=""

#----------------------------------------------------------
#  Part 1: Check for and handle command-line arguments
TIME_WARP=1
for ARGI; do
    if [ "${ARGI}" = "--help" -o "${ARGI}" = "-h" ] ; then
	echo "launch.sh [SWITCHES] [time_warp]     "
	echo "  --help, -h                         " 
	echo "  --clean_and_katm, -cl              " 
	exit 0;
    elif [ "${ARGI//[^0-9]/}" = "$ARGI" -a "$TIME_WARP" = 1 ]; then 
        TIME_WARP=$ARGI
    elif [ "${ARGI}" = "--clean_and_ktm" -o "${ARGI}" = "-cl" ] ; then
        CLL_KTM="yes"
    else 
        echo "Launch.sh Bad arg:" $ARGI " Exiting with code: 1"
	exit 1
    fi
done

if [ ${CLL_KTM} = "yes" ] ; then
    ./clean.sh
    ktm
fi


#----------------------------------------------------------
#  Part 3: Launch the processes
#----------------------------------------------------------
echo "Launching $COMMUNITY MOOS Community. WARP is" $TIME_WARP
pAntler $COMMUNITY.moos --MOOSTimeWarp=$TIME_WARP >& /dev/null &

uMAC  $COMMUNITY.moos

kill -- -$$
