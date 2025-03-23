#!/bin/bash 
#----------------------------------------------------------
#  Script: xlaunch.sh
#  Author: Michael Benjamin
#  LastEd: November 2024
#----------------------------------------------------------
# Catch SIGTERM so we dont come down with our own kill below
trap "echo xlaunch.sh has received sigterm" SIGTERM

#  Part 1: Declare global var defaults
ZBATCH=""
ARCHIVE=""
MAX_TIME="300"
FLOW_DOWN_ARGS="-x "
NOGUI=""

#  Part 2: Check for and handle command-line arguments
for ARGI; do
    if [ "${ARGI}" = "--help" -o "${ARGI}" = "-h" ]; then
	echo "xlaunch.sh [OPTIONS] [time_warp]     "
	echo "Synopsis:                                              " 
	echo "  Launch a mission, auto-started with pAutoPoke, run   "
	echo "  uMayFinish, wait for exit, bring the mission down.   " 
	echo "                                                       " 
	echo "Options:                                               " 
	echo "  --help, -h         Show this help message            " 
	echo "  --zbatch=<name>    Name of ZBatch                    "
	echo "  --max_time=<secs>  Max time passed to uMayFinish     "
	echo "  --nogui, -n        Do not launch with a GUI          "
	echo "  --archive, -a      Archive the results               "
	exit 0;
    elif [ "${ARGI//[^0-9]/}" = "$ARGI" -a "$TIME_WARP" = 1 ]; then 
	FLOW_DOWN_ARGS+="${ARGI} "
    elif [ "${ARGI}" = "--archive" -o "${ARGI}" = "-a" ]; then
        ARCHIVE="yes"
    elif [ "${ARGI}" = "--nogui" -o "${ARGI}" = "-ng" ]; then
	FLOW_DOWN_ARGS+="${ARGI} "
    elif [ "${ARGI:0:9}" = "--zbatch=" ]; then
        ZBATCH="${ARGI#--zbatch=*}"
    elif [ "${ARGI:0:11}" = "--max_time=" ]; then
        MAX_TIME="${ARGI#--max_time=*}"
    else 
	FLOW_DOWN_ARGS+="${ARGI} "
    fi
done

# Part 3: Launch mission
./launch.sh $FLOW_DOWN_ARGS
# Part 4: Monitor mission, kill MOOS processes when done
uMayFinish --max_time=${MAX_TIME} targ_shoreside.moos
# Part 5: Bring down the launched mission
kill -s SIGTERM 0
# Part 6: sleep 2 secs to let mission processes come down.
sleep 2

# Part 7:  If results file does not end in newline, add one
# Context: If pMissionEval never posts result due to mission
#          timeout, it may be configured to post partial line
if [ ! -z "$(tail -c 1 <"results.txt")" ]; then
    echo "" >> results.txt
fi

# Part 8: If post_process.sh exists, execute it
if [ -f "post_process.sh" ]; then
    echo "Executing post_process.sh..."
    ./post_process.sh
    echo "DONE Executing post_process.sh."
fi


# Part 9: Archive the results
if [ "${ARCHIVE}" != "" ]; then
    mhash_archive_dir.sh --zbatch="${ZBATCH}" --rm_logs --rm_tgzs
fi
