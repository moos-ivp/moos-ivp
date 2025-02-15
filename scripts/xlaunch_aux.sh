#!/bin/bash 
#----------------------------------------------------------
#  Script: xlaunch_aux.sh
#  Author: Michael Benjamin
#  LastEd: Jan 27th 2025
#----------------------------------------------------------
#on_exit() { echo; "Halting xlaunch_aux.sh"; kill -- -$$; }
#trap on_exit SIGINT

FLOW_DOWN_ARGS="-x "
MAX_TIME="300"

# Part 1: Check for and handle command-line arguments
for ARGI; do
    if [ "${ARGI}" = "--help" -o "${ARGI}" = "-h" ]; then
	echo "xlaunch.sh [OPTIONS]                                   "
	echo "Synopsis:                                              " 
	echo "  Launch a mission, auto-started with pAutoPokke, run  "
	echo "  uMayFinish, wait for exit, bring the mission down.   " 
	echo "                                                       " 
  	echo "Options:                                               " 
	echo "  --help, -h         Show this help message            " 
	echo "  --max_time=<secs>  Max time passed to uMayFinish     "
    elif [ "${ARGI:0:11}" = "--max_time=" ]; then
        MAX_TIME="${ARGI#--max_time=*}"
    else 
	FLOW_DOWN_ARGS+="${ARGI} "
    fi
done

# Part 2: Launch mission
./launch.sh $FLOW_DOWN_ARGS
# Part 3: Monitor mission, kill MOOS processes when done
uMayFinish --max_time=${MAX_TIME} targ_shoreside.moos
# Part 4: Bring down the launched mission
kill -s SIGTERM 0

