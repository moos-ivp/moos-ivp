#!/bin/bash
#----------------------------------------------------------
#  Script: launch.sh
#  Author: Michael Benjamin
#  LastEd: Aug 20th 2026
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
VAMT="2"
MAX_VAMT="25"

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
	echo "  --amt=N            Num vehicles to launch   "
	exit 0;
    elif [ "${ARGI//[^0-9]/}" = "$ARGI" -a "$TIME_WARP" = 1 ]; then 
        TIME_WARP=$ARGI
    elif [ "${ARGI}" = "--verbose" -o "${ARGI}" = "-v" ]; then
        VERBOSE=$ARGI
    elif [ "${ARGI}" = "--just_make" -o "${ARGI}" = "-j" ]; then
	JUST_MAKE="yes"
    elif [ "${ARGI:0:6}" = "--amt=" ]; then
        VAMT="${ARGI#--amt=*}"
	if [ $VAMT -lt 1 -o $VAMT -gt $MAX_VAMT ]; then
	    echo "$ME: Veh amt range: [1, $MAX_VAMT]. Exit Code 2."
	    exit 2
	fi
    else 
        echo "$ME Bad arg:" $ARGI " Exiting with code: 1"
        exit 1
    fi
done

#------------------------------------------------------------
#  Part 4: Set starting positions, speeds, names, colors, etc
#------------------------------------------------------------
INIT_VARS=" --amt=$VAMT $RAND_VPOS $VERBOSE "
./init_field2.sh $INIT_VARS

VEHPOS=(`cat vp_startpos.txt`)
ENDPOS=(`cat vp_endpos.txt`)
VNAMES=(`cat vp_names.txt`)
VCOLOR=(`cat vp_colors.txt`)

#----------------------------------------------------------
#  Part 4: Launch the processes
#----------------------------------------------------------
if [ "${VERBOSE}" != "" ]; then
    echo "============================================"
    echo "  launch.sh SUMMARY                         "
    echo "============================================"
    echo "$ME                                         "
    echo "CMD_ARGS =      [${CMD_ARGS}]               "
    echo "TIME_WARP =     [${TIME_WARP}]              "
    echo "JUST_MAKE =     [${JUST_MAKE}]              "
    echo "--------------------------------(VProps)----"
    echo "VNAMES =        [${VNAMES[*]}]              "
    echo "VCOLOR =        [${VCOLOR[*]}]              "
    echo "VEHPOS =        [${VEHPOS[*]}]              "
    echo "ENDPOS =        [${ENDPOS[*]}]              "
    echo "--------------------------------------------"
    echo -n "Hit any key to continue launch "
    read ANSWER
fi

ARGS="--auto $TIME_WARP $VERBOSE $JUST_MAKE"


#------------------------------------------------------------
#  Part 6: Launch the Vehicles
#------------------------------------------------------------
VARGS=" --auto $TIME_WARP $JUST_MAKE $VERBOSE "
for IX in `seq 1 $VAMT`;
do
    IXX=$(($IX - 1))
    IVARGS="$VARGS --mport=900${IX}  --pshare=920${IX} "
    IVARGS+=" --start_pos=${VEHPOS[$IXX]} "
    IVARGS+=" --end_pos=${ENDPOS[$IXX]} "
    IVARGS+=" --vname=${VNAMES[$IXX]} "
    IVARGS+=" --color=${VCOLOR[$IXX]} "
    vecho "Launching vehicle: $IVARGS"

    CMD="./launch_vehicle.sh $IVARGS"    
    eval $CMD
    sleep 0.5
done


./launch_shoreside.sh $ARGS

#----------------------------------------------------------
#  Part 5: Post Launch
#----------------------------------------------------------
uMAC -t targ_shoreside.moos
kill -- -$$
exit 0
