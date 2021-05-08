#!/bin/bash 
#----------------------------------------------------------------
#   Script: launch.sh                       
#  Mission: v3_berta_colavd
#---------------------------------------------------------------- 
#  Part 1: Define a convenience function for producing terminal
#          debugging/status output depending on the verbosity.
#-------------------------------------------------------------- 
vecho() { if [ "$VERBOSE" != "" ]; then echo "$ME: $1"; fi }

#----------------------------------------------------------------
#  Part 2: Set global var defaults
#----------------------------------------------------------------
ME=`basename "$0"`
TIME_WARP=1
JUST_MAKE=""
VERBOSE=""
AUTO_LAUNCHED="no"
CMD_ARGS=""

MISSION="v3_berta_colavd"
MAX_TIME="1000"
MAXSPD=5
WORM_HOLE=""
AMT=1

#---------------------------------------------------------------
#  Part 3: Check for and handle command-line arguments
#---------------------------------------------------------------
for ARGI; do
    if [ "${ARGI}" = "--help" -o "${ARGI}" = "-h" ]; then
	echo "$ME [SWITCHES] [WARP]                            " 
	echo "  --help, -h                                     " 
	echo "    Display this help message                    "
	echo "  --just_make, -j                                " 
	echo "    Just make targ files, but do not launch      "
	echo "  --verbose, -v                                  " 
	echo "    Verbose output, confirm before launching.    "
	echo "  --auto, -a                                     "
	echo "     Auto-launched by a script.                  "
	echo "     Will not launch uMAC as the final step.     "
	echo "                                                 "
	echo "  --max_time=<500>                               " 
	echo "    Auto-launched missions timeout after N secs  "
	echo "  --maxspd=<5>                                   " 
	echo "    Number of vehicles to launch. Default is 1.  "
	echo "  --amt=<1>                                      " 
	echo "    Number of vehicles to launch. Default is 1.  "
	echo "  --wormhole, -w                                 " 
	echo "    Enable simulation worm hole(s)               "
	exit 0;
    elif [ "${ARGI//[^0-9]/}" = "$ARGI" -a "$TIME_WARP" = 1 ]; then 
        TIME_WARP=$ARGI
    elif [ "${ARGI}" = "--just_make" -o "${ARGI}" = "-j" ]; then
        JUST_MAKE="-j"
    elif [ "${ARGI}" = "--verbose" -o "${ARGI}" = "-v" ]; then
	VERBOSE="--verbose"
    elif [ "${ARGI}" = "--auto" -o "${ARGI}" = "-a" ]; then
	AUTO_LAUNCHED="yes"

    elif [ "${ARGI:0:11}" = "--max_time=" ]; then
        MAX_TIME="${ARGI#--max_time=*}"
    elif [ "${ARGI:0:9}" = "--maxspd=" ]; then
        MAXSPD="${ARGI#--maxspd=*}"
    elif [ "${ARGI:0:6}" = "--amt=" ]; then
        AMT="${ARGI#--amt=*}"
	if [ ! $AMT -ge 1 ]; then
            echo "$ME: Vehicle amount must be >= 1."
            exit 1
        fi
    elif [ "${ARGI}" = "--wormhole" -o "${ARGI}" = "-w" ]; then
	WORM_HOLE="--wormhole"
    else
	echo "$ME: Bad Arg:" $ARGI "Exit Code 1."
	exit 1
    fi
done

#---------------------------------------------------------------
# Part 4: Generate random starting positions, speeds and vnames
#---------------------------------------------------------------
VEHPOS=(`cat vpositions.txt`)
VDESTS=(`cat vdestinations.txt`)
SPEEDS=(`cat vspeeds.txt`)
VHDGS=(`cat vheadings.txt`)
VNAMES=(`cat vnames.txt`)
VGROUPS=(`cat vgroups.txt`)
VROLES=(`cat vroles.txt`)
ALL_VNAMES=""

#---------------------------------------------------------------
# Part 5: Launch the vehicles
#---------------------------------------------------------------
for INDEX in `seq 1 $AMT`;
do  
    sleep 0.2
    ARRAY_INDEX=`expr $INDEX - 1`
    START=${VEHPOS[$ARRAY_INDEX]}
    VDEST=${VDESTS[$ARRAY_INDEX]}
    VROLE=${VROLES[$ARRAY_INDEX]}
    VNAME=${VNAMES[$ARRAY_INDEX]}
    SPEED=${SPEEDS[$ARRAY_INDEX]}
    VGRP=${VGROUPS[$ARRAY_INDEX]}
    VHDG=${VHDGS[$ARRAY_INDEX]}

    IX_VLAUNCH_ARGS="--auto --vgroup=$VGRP "
    IX_VLAUNCH_ARGS+="--index=$INDEX --start=$START "
    IX_VLAUNCH_ARGS+="--vname=$VNAME --vdest=$VDEST "
    IX_VLAUNCH_ARGS+="--spd=$SPEED   --maxspd=$MAXSPD "
    IX_VLAUNCH_ARGS+="--hdg=$VHDG    --vrole=$VROLE "
    IX_VLAUNCH_ARGS+="$WORM_HOLE $VERBOSE $JUST_MAKE $TIME_WARP "

    if [ "${ALL_VNAMES}" != "" ]; then
	ALL_VNAMES+=":"
    fi
    ALL_VNAMES+="$VNAME"
    
    vecho "Launching: $VNAME"
    vecho "IX_VLAUNCH_ARGS: [$IX_VLAUNCH_ARGS]"

    ./launch_vehicle.sh $IX_VLAUNCH_ARGS 
done

#---------------------------------------------------------------
#  Part 6: Launch the shoreside
#---------------------------------------------------------------
echo "Launching Shoreside...."
SLAUNCH_ARGS=" --auto --vnames=$ALL_VNAMES --max_time=$MAX_TIME"
SLAUNCH_ARGS+=" $JUST_MAKE $VERBOSE"
./launch_shoreside.sh $SLAUNCH_ARGS $TIME_WARP


#---------------------------------------------------------------
#  Part 7: If launched from script, we're done, exit now
#---------------------------------------------------------------
if [ "${AUTO_LAUNCHED}" = "yes" -o "${JUST_MAKE}" != "" ]; then
    exit 0
fi

#---------------------------------------------------------------
# Part 8: Launch uMAC in paused mode until the mission is quit
#---------------------------------------------------------------
uMAC --paused targ_shoreside.moos
kill -- -$$
