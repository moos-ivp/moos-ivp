#!/bin/bash 
#----------------------------------------------------------
#  Script: xlaunch.sh
#  Author: Michael Benjamin
#  LastEd: November 2024
#----------------------------------------------------------
#  Part 1: Set convenience functions for producing terminal
#          debugging output, and catching SIGTERM.
#----------------------------------------------------------
vecho() { if [ "$VERBOSE" != "" ]; then echo "$ME: $1"; fi }
trap "$ME: echo received sigterm" SIGTERM

#----------------------------------------------------------
#  Part 1: Declare global var defaults
#----------------------------------------------------------
ME=`basename "$0"`
TIME_WARP=1
VERBOSE=""
FLOW_DOWN_ARGS="-x "
ZBATCH=""
ARCHIVE=""
CLEAN="no"
MPATH=""
MISSION=""
ALL_ARGS=""
MAX_TIME="300"

#-------------------------------------------------------
#  Part 2: Check for and handle command-line arguments
#-------------------------------------------------------
for ARGI; do
    ALL_ARGS+=$ARGI" "
    if [ "${ARGI}" = "--help" -o "${ARGI}" = "-h" ]; then
	echo "xlaunch.sh [OPTIONS] [time_warp]     "
	echo "Synopsis:                                              " 
	echo "  Run from within a mission folder:                    "
	echo "    - Auto launch a mission                            " 
	echo "    - (Mission is kicked off with pAutoPoke)           " 
	echo "    - Connect with uMayFinish until mission completes  " 
	echo "    - Bring down the mission                           " 
	echo "    - Optionally generate an outcome report file       " 
	echo "                                                       " 
	echo "Options:                                               " 
	echo "  --help, -h         Show this help message            " 
	echo "  --verbose, -v      Show verbose output               " 
	echo "  --mission=<str>    Folder where mission launched from"
	echo "  --path=<str>       Locations to look for mission     "
	echo "  --clean, -c        Remove outcome file after OK send " 
	echo "  --zbatch=<name>    Name of ZBatch                    "
	echo "  --max_time=<secs>  Max time passed to uMayFinish     "
	echo "  --archive, -a      Archive the results               "
	exit 0;
    elif [ "${ARGI//[^0-9]/}" = "$ARGI" -a "$TIME_WARP" = 1 ]; then 
	TIME_WARP=$ARGI
    elif [ "${ARGI}" = "--verbose" -o "${ARGI}" = "-v" ]; then
        VERBOSE=$ARGI
    elif [ "${ARGI}" = "--res" -o "${ARGI}" = "-r" ]; then
        RESFILE="yes"
    elif [ "${ARGI}" = "--clean" -o "${ARGI}" = "-c" ]; then
        CLEAN="yes"
    elif [ "${ARGI}" = "--archive" -o "${ARGI}" = "-a" ]; then
        ARCHIVE="yes"
    elif [ "${ARGI:0:9}" = "--zbatch=" ]; then
        ZBATCH="${ARGI#--zbatch=*}"
    elif [ "${ARGI:0:7}" = "--path=" ]; then
        MPATH="${ARGI#--path=*}"
    elif [ "${ARGI:0:11}" = "--max_time=" ]; then
        MAX_TIME="${ARGI#--max_time=*}"
    elif [ "${ARGI:0:10}" = "--mission=" ]; then
        MISSION="${ARGI#--mission=*}"
    else 
	FLOW_DOWN_ARGS+="${ARGI} "
    fi
done

#-------------------------------------------------------
#  Part 3: Find mission if it is not ./ (default)
#          If mission folder named, try to find mission
#          folder based on the given path. 
#-------------------------------------------------------
FULL_PATH=""
if [ "${MISSION}" != "" ]; then
    IFS=':' read -ra DIRS <<< "$MPATH"
    for DIR in "${DIRS[@]}"
    do
	TRY_FULL_PATH="${DIR}/${MISSION}"
	vecho "try_full_path:" $TRY_FULL_PATH
	if [ -d "${TRY_FULL_PATH}" ]; then
	    FULL_PATH=$TRY_FULL_PATH
	fi
    done
fi

# Report failure if named mission cannot be found
if [ "${MISSION}" != "" -a "${FULL_PATH}" = "" ]; then
    vecho "Could not find mission: ${MISSION}"
    vecho "Path: ${MPATH}"
    vecho "Exit code 3."
    exit 3
fi



#-------------------------------------------------------
#  Part 3: Launch mission
#-------------------------------------------------------
vecho "=================================================="
vecho "xlaunch.sh: $ALL_ARGS"
vecho "=================================================="
vecho "Part 1: Launching the Mission... "

if [ "${VERBOSE}" = "" ]; then
    ./launch.sh $FLOW_DOWN_ARGS $TIME_WARP >& /dev/null
else
    ./launch.sh $FLOW_DOWN_ARGS $TIME_WARP
fi
LEXIT_CODE=$?

if [ $LEXIT_CODE != 0 ]; then
    vecho "The launch.sh cmd return non-zero exit code: $LEXIT_CODE"
    vecho "FLOW_DOWN_ARGS: "$FLOW_DOWN_ARGS
    exit 1
fi
vecho "DONE"

#-------------------------------------------------------
#  Part 4: Monitor mission, kill MOOS processes when done
#-------------------------------------------------------
uMayFinish --max_time=${MAX_TIME} targ_shoreside.moos

vecho "Part 4: Bringing down the mission... "


#trap "echo generating sigterm. This take a sec or two..." SIGTERM
#kill -s SIGTERM 0

ktm
killall -9 pAntler
rapps.sh -k


sleep 2
vecho "...DONE"

#=======================================================
#  Part 6: If post_process.sh exists, execute it
#=======================================================
if [ -f "post_process.sh" ]; then
    vecho "executing post_process.sh..."
    ./post_process.sh
    vecho "DONE executing post_process.sh."
fi

#=======================================================
#  Part 7: Archive the results
#=======================================================
if [ "${ARCHIVE}" != "" ]; then
    mhash_archive_dir.sh --zbatch="${ZBATCH}" --rm_logs --rm_tgzs
fi

#=======================================================
#  Part 8: Double-check mission is completely down 
#=======================================================
rapps.sh 
if [ $? != 0 ]; then
    rapps.sh -k
fi



