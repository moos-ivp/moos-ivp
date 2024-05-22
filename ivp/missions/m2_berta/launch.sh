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
JUST_MAKE=""
VERBOSE=""
CMD_ARGS=""

#----------------------------------------------------------
#  Part 3: Check for and handle command-line arguments
#----------------------------------------------------------
for ARGI; do
    CMD_ARGS+="${ARGI} "
    if [ "${ARGI}" = "--help" -o "${ARGI}" = "-h" ]; then
	echo "launch.sh [SWITCHES] [time_warp]    " 
	echo "  --help, -h           Show this help message            " 
	echo "  --just_make, -j      Just create targ files, no launch " 
	echo "  --verbose, -v        Verbose output, confirm launch    "
	echo "  --fast, -f           Init positions for fast encounter " 
	exit 0;
    elif [ "${ARGI//[^0-9]/}" = "$ARGI" -a "$TIME_WARP" = 1 ]; then 
        TIME_WARP=$ARGI
    elif [ "${ARGI}" = "--verbose" -o "${ARGI}" = "-v" ]; then
        VERBOSE=$ARGI 
   elif [ "${ARGI}" = "--just_make" -o "${ARGI}" = "-j" ]; then
	JUST_MAKE=$ARGI
    elif [ "${ARGI}" = "--fast" -o "${ARGI}" = "-f" ]; then
	FAST="yes"
	START_POS1="170,-80,270"         
	START_POS2="-30,-80,90"        
	LOITER_POS1="x=0,y=-95"
	LOITER_POS2="x=125,y=-65"
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
    echo "  launch.sh SUMMARY               "
    echo "=================================="
    echo "$ME"
    echo "CMD_ARGS =      [${CMD_ARGS}]     "
    echo "TIME_WARP =     [${TIME_WARP}]    "
    echo "JUST_MAKE =     [${JUST_MAKE}]    "
    echo "----------------------------------"
    echo "START_POS1 =    [${VEHPOS[0]}]    "
    echo "START_POS2 =    [${VEHPOS[1]}]    "
    echo "LOITER_POS1 =   [${LOIPOS[0]}]    "
    echo "LOITER_POS2 =   [${LOIPOS[1]}]    "
    echo "VCOLOR1 =       [${VCOLOR[0]}]    "
    echo "VCOLOR2 =       [${VCOLOR[1]}]    "
    echo "VNAME1 =        [${VNAMES[0]}]    "
    echo "VNAME2 =        [${VNAMES[1]}]    "
    echo -n "Hit any key to continue launch "
    read ANSWER
fi

#----------------------------------------------------------
#  Part 6: Launch the Vehicles
#----------------------------------------------------------
VARGS="--sim --auto $TIME_WARP $JUST_MAKE $VERBOSE "

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

if [ "${JUST_MAKE}" = "yes" ]; then
    echo "Targ files made; exiting without launch."
    exit 0
fi

#--------------------------------------------------------------
#  Part 8: Unless auto-launched, launch uMAC until mission quit
#--------------------------------------------------------------
if [ "${XLAUNCHED}" = "no" ]; then
    uMAC targ_shoreside.moos
    kill -- -$$
fi

exit 0
