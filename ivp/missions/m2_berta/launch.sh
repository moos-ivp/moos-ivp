#!/bin/bash -e
#----------------------------------------------------------
#   Script: launch.sh
#  Mission: m2_berta
#   Author: Michael Benjamin
#   LastEd: May 17th 2019
#--------------------------------------------------------------
#  Part 1: Define a convenience function for producing terminal
#          debugging/status output depending on the verbosity.
#--------------------------------------------------------------
vecho() { if [ "$VERBOSE" != "" ]; then echo "$ME: $1"; fi }

#--------------------------------------------------------------
#  Part 2: Set Global variables
#-------------------------------------------------------------=
ME=`basename "$0"`
TIME_WARP=1
VERBOSE=""
JUST_MAKE=""
CMD_ARGS=""
XLAUNCHED=""
NOGUI=""

#----------------------------------------------------------
#  Part 3: Check for and handle command-line arguments
#----------------------------------------------------------
for ARGI; do
    CMD_ARGS+="${ARGI} "
    if [ "${ARGI}" = "--help" -o "${ARGI}" = "-h" ]; then
	echo "$ME [OPTIONS] [time_warp]                    " 
	echo "  --help, -h         Show this help message  " 
	echo "  --just_make, -j    Only create targ files  " 
	echo "  --verbose, -v      Verbose, confirm launch "
	echo "  --xlaunched, -x    Launched by xlaunch     "
	echo "  --nogui, -ng       Headless launch, no gui "
	echo "  --fast, -f         Start head-on encounter " 
	exit 0;
    elif [ "${ARGI//[^0-9]/}" = "$ARGI" -a "$TIME_WARP" = 1 ]; then 
        TIME_WARP=$ARGI
    elif [ "${ARGI}" = "--verbose" -o "${ARGI}" = "-v" ]; then
        VERBOSE=$ARGI 
    elif [ "${ARGI}" = "--just_make" -o "${ARGI}" = "-j" ]; then
	JUST_MAKE=$ARGI
    elif [ "${ARGI}" = "--xlaunched" -o "${ARGI}" = "-x" ]; then
	XLAUNCHED="yes"
    elif [ "${ARGI}" = "--nogui" -o "${ARGI}" = "-ng" ]; then
	NOGUI="--nogui"
    elif [ "${ARGI}" = "--fast" -o "${ARGI}" = "-f" ]; then
	FAST="yes"
    else 
        echo "launch.sh Bad arg:" $ARGI " Exiting with code: 1"
        exit 1
    fi
done

#-------------------------------------------------------------
# Part 4: Set starting positions, vnames, colors
#-------------------------------------------------------------
vecho "Setting starting position and colors"

VEHPOS=("0,0,180"   "180,0,180")
LOIPOS=("x=0,y=-75" "x=125,y=-50")
if [ "${FAST}" = "yes" ]; then
    VEHPOS=("170,-80,270" "-30,-80,90")
    LOIPOS=("x=0,y=-95"  "x=125,y=-65")
fi
VNAMES=("abe" "ben")
VCOLOR=("red" "yellow")

#---------------------------------------------------------------
#  Part 5: If verbose, show vars and confirm before launching
#---------------------------------------------------------------
if [ "${VERBOSE}" != "" ]; then
    echo "=================================="
    echo "  $ME SUMMARY              (ALL)  "
    echo "=================================="
    echo "CMD_ARGS =      [${CMD_ARGS}]     "
    echo "TIME_WARP =     [${TIME_WARP}]    "
    echo "JUST_MAKE =     [${JUST_MAKE}]    "
    echo "XLAUNCHED =     [${XLAUNCHED}]    "
    echo "NOGUI =         [${NOGUI}]        "
    echo "----------------------------------"
    echo "START_POS =     [${VEHPOS[*]}]    "
    echo "LOITER_POS  =   [${LOIPOS[*]}]    "
    echo "VCOLORS =       [${VCOLOR[*]}]    "
    echo "VNAMES =        [${VNAMES[*]}]    "
    echo -n "Hit any key to continue launch "
    read ANSWER
fi

#----------------------------------------------------------
#  Part 6: Launch the Vehicles
#----------------------------------------------------------
VARGS="--sim --auto $TIME_WARP $JUST_MAKE $VERBOSE $NOGUI"

AMT="2"
for IX in `seq 1 $AMT`;
do
    IXX=$(($IX - 1))
    IVARGS="$VARGS --mport=900${IX}  --pshare=920${IX} "
    IVARGS+=" --vname=${VNAMES[$IXX]}  "
    IVARGS+=" --start=${VEHPOS[$IXX]}  "
    IVARGS+=" --color=${VCOLOR[$IXX]}  "
    IVARGS+=" --loipos=${LOIPOS[$IXX]} "
    vecho "IVARGS: $IVARGS"
    vecho "Launching vehicle: $IVARGS"
    ./launch_vehicle.sh $IVARGS; sleep 0.5
done

#----------------------------------------------------------
#  Part 7: Launch the Shoreside
#----------------------------------------------------------
SARGS="--mport=9000 --pshare=9200 --auto --vnames=abe:ben "
SARGS+="$TIME_WARP $JUST_MAKE $VERBOSE"
vecho "Launching shoreside: $SARGS"
./launch_shoreside.sh $SARGS 

if [ "${JUST_MAKE}" != "" ]; then
    echo "Targ files made; exiting without launch."
    exit 0
fi

#--------------------------------------------------------------
#  Part 8: Unless auto-launched, launch uMAC until mission quit
#--------------------------------------------------------------
if [ "${XLAUNCHED}" != "yes" ]; then
    uMAC targ_shoreside.moos
    kill -- -$$
fi

exit 0
