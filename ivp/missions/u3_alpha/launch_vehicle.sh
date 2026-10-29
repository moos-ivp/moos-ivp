#!/bin/bash
#----------------------------------------------------------
#  Script: launch_vehicle.sh
#  Author: Michael Benjamin
#  LastEd: Oct 20th 2026
#----------------------------------------------------------         
#  Part 1: Set convenience functions for producing terminal
#          debugging output, and catching SIGINT (ctrl-c).
#----------------------------------------------------------
vecho() { if [ "$VERBOSE" != "" ]; then echo "$ME: $1"; fi }
on_exit() { echo; echo "$ME: Halting all apps"; kill -- -$$; }
trap on_exit SIGINT
trap '' TERM

#----------------------------------------------------------
#  Part 2: Set global variable default values  
#----------------------------------------------------------
ME=`basename "$0"`
TIME_WARP=1
VERBOSE=""
AUTO_LAUNCHED="no"
VNAME="alpha"

#----------------------------------------------------------
#  Part 3: Check for and handle command-line arguments
#----------------------------------------------------------
for ARGI; do
    CMD_ARGS+="${ARGI} "
    if [ "${ARGI}" = "--help" -o "${ARGI}" = "-h" ] ; then
	echo "$ME: [OPTIONS] [time_warp]                    "
	echo "                                              "
	echo "Options:                                      "
	echo "  --help, -h         Show this help message   "
	echo "  --verbose, -v      Verbose, confirm launch  "
        echo "  --auto, -a         Script launch, no uMAC   "
        echo "                                              "
	echo "  --vname=<alpha>    Vehicle name given       "

	exit 0;
    elif [ "${ARGI//[^0-9]/}" = "$ARGI" -a "$TIME_WARP" = 1 ]; then 
        TIME_WARP=$ARGI
    elif [ "${ARGI}" = "--verbose" -o "${ARGI}" = "-v" ]; then
        VERBOSE=$ARGI
    elif [ "${ARGI}" = "--auto" -o "${ARGI}" = "-a" ]; then
        AUTO_LAUNCHED="yes"
    elif [ "${ARGI:0:8}" = "--vname=" ]; then
        VNAME="${ARGI#--vname=*}"
    else 
        echo "$ME Bad arg:" $ARGI " Exiting with code: 1"
        exit 1
    fi
done


#----------------------------------------------------------
#  Part 4: Launch the processes
#----------------------------------------------------------
if [ "${VERBOSE}" != "" ]; then
    echo "=================================="
    echo "  launch_vehicle.sh SUMMARY       "
    echo "=================================="
    echo "$ME                               "
    echo "CMD_ARGS =      [${CMD_ARGS}]     "
    echo "TIME_WARP =     [${TIME_WARP}]    "
    echo "AUTO_LAUNCHED = [${AUTO_LAUNCHED}]"
    echo "VNAME =         [${VNAME}]        "
    echo "----------------------------------"
    echo -n "Hit any key to continue launch "
    read ANSWER
fi

echo "Launching $VNAME MOOS Community with WARP:" $TIME_WARP
pAntler $VNAME.moos --MOOSTimeWarp=$TIME_WARP >& /dev/null &

#----------------------------------------------------------
#  Part 5: Post Launch
#----------------------------------------------------------
if [ "${AUTO_LAUNCHED}" = "yes" ]; then
    exit 0
fi

uMAC -t $VNAME.moos
kill -- -$$
exit 0
