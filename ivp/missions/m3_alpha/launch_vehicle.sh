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
JUST_MAKE=""
AUTO_LAUNCHED="no"

MOOS_PORT="9001"
PSHARE_PORT="9001"
VNAME="abe"
COLOR="yellow"

#----------------------------------------------------------
#  Part 3: Check for and handle command-line arguments
#----------------------------------------------------------
for ARGI; do
    CMD_ARGS+="${ARGI} "
    if [ "${ARGI}" = "--help" -o "${ARGI}" = "-h" ] ; then
	echo "$ME: [OPTIONS] [time_warp]                      "
	echo "                                                "
	echo "Options:                                        "
	echo "  --help, -h         Show this help message     "
	echo "  --just_make, -j    Only create targ files  "
	echo "  --verbose, -v      Verbose, confirm launch    "
        echo "  --auto, -a         Script launch, no uMAC     "
        echo "                                                "
	echo "  --mport=<9001>     Vehicle MOOSDB port        "
	echo "  --pshare=<9201>    Vehicle pShare listen port "
        echo "                                                "
	echo "  --vname=<abe>      Vehicle name given         "
	echo "  --vcolor=<yellow>  Vehicle name given         "
	echo "  --start_pos=<X,Y,H,V>  Sim start pos/hdg      "
	echo "  --end_pos=<X,Y>    Sim end pos                "
	exit 0;
    elif [ "${ARGI//[^0-9]/}" = "$ARGI" -a "$TIME_WARP" = 1 ]; then 
        TIME_WARP=$ARGI
    elif [ "${ARGI}" = "--verbose" -o "${ARGI}" = "-v" ]; then
        VERBOSE=$ARGI
    elif [ "${ARGI}" = "--just_make" -o "${ARGI}" = "-j" ]; then
	JUST_MAKE="yes"
    elif [ "${ARGI}" = "--auto" -o "${ARGI}" = "-a" ]; then
        AUTO_LAUNCHED="yes"
    elif [ "${ARGI:0:8}" = "--mport=" ]; then
	MOOS_PORT="${ARGI#--mport=*}"
    elif [ "${ARGI:0:9}" = "--pshare=" ]; then
        PSHARE_PORT="${ARGI#--pshare=*}"
    elif [ "${ARGI:0:8}" = "--vname=" ]; then
        VNAME="${ARGI#--vname=*}"
    elif [ "${ARGI:0:8}" = "--color=" ]; then
        COLOR="${ARGI#--color=*}"
    elif [ "${ARGI:0:8}" = "--vpath=" ]; then
	VPATH="${ARGI#--vpath=*}"
    elif [ "${ARGI:0:12}" = "--start_pos=" ]; then
        START_POS="${ARGI#--start_pos=*}"
    elif [ "${ARGI:0:10}" = "--end_pos=" ]; then
        END_POS="${ARGI#--end_pos=*}"
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
    echo "JUST_MAKE =     [${JUST_MAKE}]    "
    echo "AUTO_LAUNCHED = [${AUTO_LAUNCHED}]"
    echo "----------------------------------"
    echo "MOOS_PORT =     [${MOOS_PORT}]    "
    echo "PSHARE_PORT =   [${PSHARE_PORT}]  "
    echo "----------------------------------"
    echo "VNAME =         [${VNAME}]        "
    echo "COLOR =         [${COLOR}]        "
    echo "START_POS =     [${START_POS}]    "
    echo "END_POS =       [${END_POS}]    "
    echo "----------------------------------"
    echo "VPATH =         [${VPATH}]        "
    echo "----------------------------------"
    echo -n "Hit any key to continue launch "
    read ANSWER
fi

#----------------------------------------------------------
# Part 5: Create the .moos and .bhv files
#----------------------------------------------------------

nsplug meta_vehicle.moos targ_$VNAME.moos --strict --force \
       WARP=$TIME_WARP           \
       VNAME=$VNAME              \
       MOOS_PORT=$MOOS_PORT      \
       PSHARE_PORT=$PSHARE_PORT  \
       START_POS=$START_POS      \
       COLOR=$COLOR

nsplug meta_vehicle.bhv targ_$VNAME.bhv --strict --force \
       END_POS=$END_POS          \
       VPATH=$VPATH

if [ "${JUST_MAKE}" = "yes" ]; then
    echo "$ME: Targ files made; exiting without launch."
    exit 0
fi

#----------------------------------------------------------
#  Part 6: Launch the vehicle mission
#----------------------------------------------------------
pAntler targ_$VNAME.moos >& /dev/null &

#----------------------------------------------------------
#  Part 7: Post Launch
#----------------------------------------------------------
if [ "${AUTO_LAUNCHED}" = "yes" ]; then
    exit 0
fi

uMAC -t $VNAME.moos
kill -- -$$
exit 0
