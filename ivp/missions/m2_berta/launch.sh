#!/bin/bash -e
#------------------------------------------------------------
#   Script: launch.sh
#  Mission: m2_berta
#   Author: M.Benjamin
#   LastEd: May 2024
#------------------------------------------------------------
#  Part 1: A convenience function for producing terminal
#          debugging/status output depending on verbosity.
#------------------------------------------------------------
vecho() { if [ "$VERBOSE" != "" ]; then echo "$ME: $1"; fi }

#------------------------------------------------------------
#  Part 2: Set global variable default values
#------------------------------------------------------------
ME=`basename "$0"`
CMD_ARGS=""
TIME_WARP=1
VERBOSE=""
JUST_MAKE=""
LOG_CLEAN=""
VAMT="2"
MAX_VAMT="2"
RAND_VPOS=""

# Monte
XLAUNCHED="no"
NOGUI=""

# Custom

#------------------------------------------------------------
#  Part 3: Check for and handle command-line arguments
#------------------------------------------------------------
for ARGI; do
    CMD_ARGS+=" ${ARGI}"
    if [ "${ARGI}" = "--help" -o "${ARGI}" = "-h" ]; then
	echo "$ME [OPTIONS] [time_warp]                      "
	echo "                                               "
	echo "Options:                                       "
	echo "  --help, -h         Show this help message    " 
	echo "  --verbose, -v      Verbose, confirm launch   "
	echo "  --just_make, -j    Only create targ files    " 
	echo "  --log_clean, -lc   Run clean.sh bef launch   " 
	echo "  --amt=N            Num vehicles to launch    "
	echo "  --rand, -r         Rand vehicle positions    "
	echo "                                               "
	echo "Options (monte):                               "
	echo "  --xlaunched, -x    Launched by xlaunch       "
	echo "  --nogui, -ng       Headless launch, no gui   "
	echo "                                               "
	echo "Options (custom):                              "
	echo "  --fast, -f         Start head-on encounter   " 
	exit 0;
    elif [ "${ARGI//[^0-9]/}" = "$ARGI" -a "$TIME_WARP" = 1 ]; then 
        TIME_WARP=$ARGI
    elif [ "${ARGI}" = "--verbose" -o "${ARGI}" = "-v" ]; then
	VERBOSE=$ARGI
    elif [ "${ARGI}" = "--just_make" -o "${ARGI}" = "-j" ]; then
	JUST_MAKE=$ARGI
    elif [ "${ARGI}" = "--log_clean" -o "${ARGI}" = "-lc" ]; then
	LOG_CLEAN=$ARGI
    elif [ "${ARGI:0:6}" = "--amt=" ]; then
        VAMT="${ARGI#--amt=*}"
	if [ $VAMT -lt 1 -o $VAMT -gt $MAX_VAMT ]; then
	    echo "$ME: Veh amt range: [1, $MAX_VAMT]. Exit Code 2."
	    exit 2
	fi
    elif [ "${ARGI}" = "--rand" -o "${ARGI}" = "-r" ]; then
        RAND_VPOS=$ARGI

    elif [ "${ARGI}" = "--xlaunched" -o "${ARGI}" = "-x" ]; then
	XLAUNCHED="yes"
    elif [ "${ARGI}" = "--nogui" -o "${ARGI}" = "-ng" ]; then
	NOGUI="--nogui"
    elif [ "${ARGI}" = "--fast" -o "${ARGI}" = "-f" ]; then
	FAST=$ARGI
    else 
	echo "$ME: Bad Arg: $ARGI. Exit Code 1."
	exit 1
    fi
done

#------------------------------------------------------------
#  Part 4: Set starting positions, speeds, vnames, colors
#------------------------------------------------------------
INIT_VARS=" --amt=$VAMT $RAND_VPOS $VERBOSE "
INIT_VARS+=" $FAST " #custom
./init_field.sh $INIT_VARS

VEHPOS=(`cat vpositions.txt`)
SPEEDS=(`cat vspeeds.txt`)
VNAMES=(`cat vnames.txt`)
VCOLOR=(`cat vcolors.txt`)
LOIPOS=(`cat vloiterpos.txt`)

#------------------------------------------------------------
#  Part 5: If verbose, show vars and confirm before launching
#------------------------------------------------------------
if [ "${VERBOSE}" != "" ]; then
    echo "============================================"
    echo "  $ME SUMMARY                   (ALL)       "
    echo "============================================"
    echo "CMD_ARGS =      [${CMD_ARGS}]               "
    echo "TIME_WARP =     [${TIME_WARP}]              "
    echo "JUST_MAKE =     [${JUST_MAKE}]              "
    echo "LOG_CLEAN =     [${LOG_CLEAN}]              "
    echo "VAMT =          [${VAMT}]                   "
    echo "MAX_VAMT =      [${MAX_VAMT}]               "
    echo "RAND_VPOS =     [${RAND_VPOS}]              "
    echo "--------------------------------(VProps)----"
    echo "VNAMES =        [${VNAMES[*]}]              "
    echo "VCOLORS =       [${VCOLOR[*]}]              "
    echo "START_POS =     [${VEHPOS[*]}]              "
    echo "--------------------------------(Monte)-----"
    echo "XLAUNCHED =     [${XLAUNCHED}]              "
    echo "NOGUI =         [${NOGUI}]                  "
    echo "--------------------------------(Custom)----"
    echo "LOITER_POS  =   [${LOIPOS[*]}]              "
    echo "                                            "
    echo -n "Hit any key to continue launch           "
    read ANSWER
fi

#------------------------------------------------------------
#  Part 6: Launch the Vehicles
#------------------------------------------------------------
VARGS=" --sim --auto $TIME_WARP $JUST_MAKE $VERBOSE "
for IX in `seq 1 $VAMT`;
do
    IXX=$(($IX - 1))
    IVARGS="$VARGS --mport=900${IX}  --pshare=920${IX} "
    IVARGS+=" --start_pos=${VEHPOS[$IXX]} "
    IVARGS+=" --stock_spd=${SPEEDS[$IXX]} "
    IVARGS+=" --vname=${VNAMES[$IXX]} "
    IVARGS+=" --color=${VCOLOR[$IXX]} "
    IVARGS+=" --loipos=${LOIPOS[$IXX]}"
    vecho "Launching vehicle: $IVARGS"

    CMD="./launch_vehicle.sh $IVARGS"    
    eval $CMD
    sleep 0.5
done

#------------------------------------------------------------
#  Part 7: Launch the Shoreside mission file
#------------------------------------------------------------
SARGS=" --auto --mport=9000 --pshare=9200 $NOGUI --vnames=abe:ben "
SARGS+=" $TIME_WARP $JUST_MAKE $VERBOSE "
vecho "Launching shoreside: $SARGS"
./launch_shoreside.sh $SARGS 

if [ "${JUST_MAKE}" != "" ]; then
    echo "$ME: Targ files made; exiting without launch."
    exit 0
fi

#------------------------------------------------------------
#  Part 8: Unless auto-launched, launch uMAC until mission quit
#------------------------------------------------------------
if [ "${XLAUNCHED}" != "yes" ]; then
    uMAC --paused targ_shoreside.moos
    kill -- -$$
fi

exit 0
