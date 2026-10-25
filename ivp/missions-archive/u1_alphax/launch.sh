#!/bin/bash
#----------------------------------------------------------
#  Script: launch.sh
#  Author: Michael Benjamin
#  LastEd: May 20th 2024
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
	exit 0;
    elif [ "${ARGI//[^0-9]/}" = "$ARGI" -a "$TIME_WARP" = 1 ]; then 
        TIME_WARP=$ARGI
    elif [ "${ARGI}" = "--verbose" -o "${ARGI}" = "-v" ]; then
        VERBOSE=$ARGI
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
    echo "  launch.sh SUMMARY               "
    echo "=================================="
    echo "$ME                               "
    echo "CMD_ARGS =      [${CMD_ARGS}]     "
    echo "TIME_WARP =     [${TIME_WARP}]    "
    echo "----------------------------------"
    echo -n "Hit any key to continue launch "
    read ANSWER
fi

./launch_shoreside.sh --auto $TIME_WARP $VERBOSE
./launch_vehicle.sh --auto $TIME_WARP $VERBOSE

#----------------------------------------------------------
#  Part 5: Post Launch
#----------------------------------------------------------
uMAC -t alpha.moos
kill -- -$$
exit 0
