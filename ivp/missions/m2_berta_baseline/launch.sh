#!/bin/bash -e
#---------------------------------------------------------------
#   Script: launch.sh
#  Mission:
#   Author:
#   LastEd: 2021-May-10
#---------------------------------------------------------------
#  Part 1: Set global var defaults
#---------------------------------------------------------------
ME=`basename "$0"`
TIME_WARP=1
JUST_MAKE=""
VERBOSE=""
AUTO_LAUNCHED="no"
NOGUI=""

#---------------------------------------------------------------
#  Part 2: Check for and handle command-line arguments
#---------------------------------------------------------------
for ARGI; do
    if [ "${ARGI}" = "--help" -o "${ARGI}" = "-h" ]; then
	echo "$ME [SWITCHES] [time_warp]                                   "
	echo "  --help, -h           Show this help message                "
	echo "  --just_make, -j      Just make targ files, no launch       "
	echo "  --verbose, -v        Verbose output, confirm before launch "
	echo "  --nogui, -n          Headless mode - no pMarineViewer etc  "
	exit 0
    elif [ "${ARGI//[^0-9]/}" = "$ARGI" -a "$TIME_WARP" = 1 ]; then
        TIME_WARP=$ARGI
    elif [ "${ARGI}" = "--just_make" -o "${ARGI}" = "-j" ]; then
	JUST_MAKE="-j"
    elif [ "${ARGI}" = "--verbose" -o "${ARGI}" = "-v" ]; then
	VERBOSE="--verbose"
    elif [ "${ARGI}" = "--nogui" -o "${ARGI}" = "-n" ]; then
	NOGUI="--nogui"
    else
        echo "$ME: Bad arg:" $ARGI "Exit Code 1."
        exit 1
    fi
done

#---------------------------------------------------------------
#  Part 3: Initialize and Launch the vehicles
#---------------------------------------------------------------
VNAME1="henry"
VNAME2="gilda"

VLAUNCH_ARGS=" --auto --vname=$VNAME1 --index=1 --start=170,-80 "
VLAUNCH_ARGS+="--hdg=270 --spd=0 --loiter_pos=x=0,y=-95       "
echo "$ME: Launching $VNAME1 ..."
./launch_vehicle.sh $VLAUNCH_ARGS $VERBOSE $JUST_MAKE $TIME_WARP

VLAUNCH_ARGS=" --auto --vname=$VNAME2 --index=2 --start=-30,-80 "
VLAUNCH_ARGS+="--hdg=90 --spd=0 --loiter_pos=x=125,y=-65      "
echo "$ME: Launching $VNAME2 ..."la
./launch_vehicle.sh $VLAUNCH_ARGS $VERBOSE $JUST_MAKE $TIME_WARP

#---------------------------------------------------------------
#  Part 4: Launch the shoreside
#---------------------------------------------------------------
echo "$ME: Launching Shoreside ..."
SLAUNCH_ARGS=" --auto --vnames=$VNAME1:$VNAME2 $NOGUI "
./launch_shoreside.sh $SLAUNCH_ARGS $VERBOSE $JUST_MAKE $TIME_WARP

#---------------------------------------------------------------
#  Part 5: If launched from script, we're done, exit now
#---------------------------------------------------------------
if [ "${AUTO_LAUNCHED}" = "yes" -o "${JUST_MAKE}" != "" ]; then
    exit 0
fi

#---------------------------------------------------------------
# Part 6: Launch uMAC until the mission is quit
#---------------------------------------------------------------
uMAC targ_shoreside.moos
kill -- -$$

