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
JUST_MAKE=""

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
	echo "  --just_make, -j    Only create targ files   "
	echo "  --verbose, -v      Verbose, confirm launch  "
	exit 0;
    elif [ "${ARGI//[^0-9]/}" = "$ARGI" -a "$TIME_WARP" = 1 ]; then 
        TIME_WARP=$ARGI
    elif [ "${ARGI}" = "--verbose" -o "${ARGI}" = "-v" ]; then
        VERBOSE=$ARGI
    elif [ "${ARGI}" = "--just_make" -o "${ARGI}" = "-j" ]; then
	JUST_MAKE=$ARGI
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
    echo "JUST_MAKE =     [${JUST_MAKE}]    "
    echo "----------------------------------"
    echo -n "Hit any key to continue launch "
    read ANSWER
fi

ARGS="--auto $TIME_WARP $VERBOSE $JUST_MAKE"
POS1="x=0,y=-20,heading=180,speed=0"
POS2="x=20,y=-20,heading=180,speed=0"
PATH1="60,-40:60,-160:150,-160:180,-100:150,-40"
PATH2="80,-60:80,-180:170,-180:200,-120:170,-60"

./launch_vehicle.sh $ARGS --vname=abe --mport=9001 --pshare=9201 \
		    --start_pos=$POS1 --vpath=$PATH1

./launch_vehicle.sh $ARGS --vname=ben --mport=9002 --pshare=9202 \
		    --start_pos=$POS2 --vpath=$PATH2

./launch_shoreside.sh $ARGS

if [ "${JUST_MAKE}" != "" ]; then
    echo "$ME: Targ files made; exiting without launch."
    exit 0
fi

#----------------------------------------------------------
#  Part 5: Post Launch
#----------------------------------------------------------
uMAC -t targ_shoreside.moos
kill -- -$$
exit 0
