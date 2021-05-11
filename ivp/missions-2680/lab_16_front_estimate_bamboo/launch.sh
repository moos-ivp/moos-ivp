#!/bin/bash -e
#---------------------------------------------------------------  
#  Part 1: Declare global var defaults
#---------------------------------------------------------------
#   Script: launch.sh
#  Mission: lab_16
#   LastEd: 2021-May-10
#---------------------------------------------------------------
#  Part 1: Set global var defaults
#---------------------------------------------------------------
ME=`basename "$0"`
TIME_WARP=1
JUST_MAKE=""
VERBOSE=""
AUTO_LAUNCHED="no"

# parameters to vehicles
COOL_FAC="--cool=50"
COOL_STEPS="--steps=1000"
UNCONCURRENT=""
ADAPTIVE=""
DEGREES1=270
DEGREES2=0

SURVEY_X=30
SURVEY_Y=-50
HEIGHT1=50
HEIGHT2=50
WIDTH1=50
WIDTH2=50
LANE_WIDTH1=10
LANE_WIDTH2=10

#---------------------------------------------------------------
#  Part 1: Check for and handle command-line arguments
#---------------------------------------------------------------
for ARGI; do
    if [ "${ARGI}" = "--help" -o "${ARGI}" = "-h" ]; then 
	echo "$ME [SWITCHES] [time_warp]                                   "
	echo "  --help, -h           Show this help message                "
	echo "  --just_make, -j      Just make targ files, no launch       "
	echo "  --verbose, -v        Verbose output, confirm before launch "
	echo "                                                             "
	echo "  --adaptive, -a                 "
	echo "  --unconcurrent, -uc            "
	echo "  --cool=COOL_FAC                "
	echo "  --steps=COOL_STEPS             "
	exit 0
    elif [ "${ARGI//[^0-9]/}" = "$ARGI" -a "$TIME_WARP" = 1 ]; then 
        TIME_WARP=$ARGI
    elif [ "${ARGI}" = "--just_make" -o "${ARGI}" = "-j" ]; then
	JUST_MAKE="yes"
    elif [ "${ARGI}" = "--verbose" -o "${ARGI}" = "-v" ]; then
	VERBOSE="-v"
	
    elif [ "${ARGI:0:7}" = "--cool=" ]; then
        COOL_FAC=$ARGI
    elif [ "${ARGI:0:8}" = "--steps=" ]; then
        COOL_STEPS=$ARGI
    elif [ "${ARGI}" = "--adaptive" -o "${ARGI}" = "-a" ]; then
        ADAPTIVE=$ARGI
    elif [ "${ARGI}" = "--unconcurrent" -o "${ARGI}" = "-uc" ]; then
        UNCONCURRENT=$ARGI
    else 
	echo "Bad Arg:[$ARGI]. Exit Code 1."
	exit 1
    fi
done

#---------------------------------------------------------------
#  Part 3: Create the .moos and .bhv files. 
#---------------------------------------------------------------
VNAME1="archie"   
VNAME2="betty"

START_POS1="0,0"  
START_POS2="10,-5"

VLAUNCH_ARGS=" --auto $COOL_FAC $COOL_STEPS $UNCONCURRENT $ADAPTIVE "
VLAUNCH_ARGS+=" --survey_x=$SURVEY_X --survey_y=$SURVEY_Y "


VLAUNCH_ARGS1="$VLAUNCH_ARGS --vname=$VNAME1 --index=1 --start=$START_POS1 "
VLAUNCH_ARGS1+="--angle=$DEGREES1 --width=$WIDTH1 "
VLAUNCH_ARGS1+="--height=$HEIGHT1 --lwidth=$LANE_WIDTH1"

echo "$ME: Launching $VNAME1 ..."
./launch_vehicle.sh $VLAUNCH_ARGS1 $VERBOSE $JUST_MAKE $TIME_WARP

VLAUNCH_ARGS2="$VLAUNCH_ARGS --vname=$VNAME2 --index=2 --start=$START_POS2 "
VLAUNCH_ARGS2+="--angle=$DEGREES2 --width=$WIDTH2 "
VLAUNCH_ARGS2+="--height=$HEIGHT2 --lwidth=$LANE_WIDTH2"

echo "$ME: Launching $VNAME2 ..."
./launch_vehicle.sh $VLAUNCH_ARGS2 $VERBOSE $JUST_MAKE $TIME_WARP

#---------------------------------------------------------------
#  Part 4: Launch the shoreside
#---------------------------------------------------------------
echo "$ME: Launching Shoreside ..."
./launch_shoreside.sh --auto $VERBOSE $JUST_MAKE $TIME_WARP

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

