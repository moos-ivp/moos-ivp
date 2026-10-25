#!/bin/bash -e
#----------------------------------------------------------
#  Script: launch.sh
#  Author: Michael Benjamin
#  LastEd: May 20th 2019
#----------------------------------------------------------
#  Part 1: Set Exit actions and declare global var defaults
#----------------------------------------------------------
trap "kill -- -$$" EXIT SIGTERM SIGHUP SIGINT SIGKILL
TIME_WARP=1
COMMUNITY="alpha"
GUI="yes"
RUN_TIME=""

#----------------------------------------------------------
#  Part 2: Check for and handle command-line arguments
#----------------------------------------------------------
for ARGI; do
    if [ "${ARGI}" = "--help" -o "${ARGI}" = "-h" ] ; then
	echo "launch.sh [SWITCHES] [time_warp]   "
	echo "  --help, -h      This help message               " 
	echo "  --nogui         No pMarineViewer and auto start "
	echo "  --rt=<secs>     Auto-kill after given time      "
	exit 0;
    elif [ "${ARGI}" = "--nogui" ] ; then
	GUI="no"
    elif [ "${ARGI:0:5}" = "--rt=" ] ; then
        RUN_TIME="${ARGI#--rt=*}"
    elif [ "${ARGI//[^0-9]/}" = "$ARGI" -a "$TIME_WARP" = 1 ]; then 
        TIME_WARP=$ARGI
    else 
        echo "launch.sh Bad arg:" $ARGI " Exiting with code: 1"
        exit 1
    fi
done

#----------------------------------------------------------
#  Part 3: Create the mission file
#----------------------------------------------------------
nsplug meta_alpha.moos targ_alpha.moos -i -f WARP=$TIME_WARP \
       GUI=$GUI

nsplug meta_alpha.bhv targ_alpha.bhv -i -f  GUI=$GUI

#----------------------------------------------------------
#  Part 4: Launch the processes
#----------------------------------------------------------
echo "Launching alpha MOOS Community. WARP is" $TIME_WARP "("$$")"
pAntler targ_alpha.moos >& /dev/null &

if [ "${RUN_TIME}" != "" ] ; then
    death_row.sh --pid=$$ --sec=$RUN_TIME &
fi

#uMAC --node=alpha --proc=uMemWatch -t targ_alpha.moos
uXMS targ_alpha.moos PHELMIVP_MEM                \
     HELM_CORE_SIZE  HELM_CORE_SIZE_MAX          \
     HELM_CORE1_SIZE HELM_CORE1_SIZE_MAX         \
     HELM_CORE2_SIZE HELM_CORE2_SIZE_MAX
