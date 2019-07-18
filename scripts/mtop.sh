#!/bin/bash 
#--------------------------------------------------------------
#  Script: mtop.sh
#  Author: Michael Benjamin
#  Date:   May 17th 2019
#  About:  This script is a convenience script for launching top
#          for monitoring MOOS apps. The primary launch mode (no
#          args), will look for all Antler processes, and find
#          all child processes. It may also be configured to run
#          on only a given set of apps named by the caller.
#--------------------------------------------------------------

#-------------------------------------------------------
#  Part 1: Initialize global variables
#-------------------------------------------------------
VERBOSE="no"
ANTLER="no"
SORTBY="cpu"
APPS=""
OS="osx"

#-------------------------------------------------------
#  Part 2: Check for and handle command-line arguments
#-------------------------------------------------------
for ARGI; do
    if [ "${ARGI}" = "--help" -o "${ARGI}" = "-h" ] ; then
        echo "mtop.sh [OPTIONS]                                       "
	echo "                                                        "
	echo "SYNOPSIS:                                               "
	echo "  This script is a convenience script for launching top "
        echo "  for monitoring MOOS apps. The primary launch mode (no "
        echo "  args, will look for all Antler processes, and find    "
        echo "  all child processes. It may also be configured to run "
        echo "  on only a given set of apps named by the caller.      "
	echo "                                                        "
        echo "Options:                                                "
        echo "  --verbose, -v      Increase verbosity                 " 
        echo "  --help,    -h      Display this help message          " 
        echo "  --mem,     -m      Sort top output by Memory          " 
        echo "  --core,    -c      Form app list with common biggies  " 
        echo "  --info,    -i      Output brief description of script "  
        echo "  --apps=<apps>      Form app list by given apps        " 
	echo "                                                        "
	echo "Examples:                                               "
 	echo "  $ mtop.sh                                             "
	echo "  $ mtop.sh --apps=pHelmIvP,pFooBar                     "
	echo "  $ mtop.sh -c -m                                       "
        exit 0;
    elif [ "${ARGI}" = "--verbose" -o "${ARGI}" = "-v" ] ; then
	VERBOSE="yes"
    elif [ "${ARGI}" = "--info"   -o "${ARGI}" = "-i" ] ; then
	echo "Run the top command showing only MOOS Apps"
	exit 0
    elif [ "${ARGI}" = "--antler" -o "${ARGI}" = "-a" ] ; then
	ANTLER="yes"
    elif [ "${ARGI}" = "--core" -o "${ARGI}" = "-c" ] ; then
	APPS="pHelmIvP,MOOSDB,pMarineViewer,uSimMarine,pMarinePID"
    elif [ "${ARGI}" = "--mem" -o "${ARGI}" = "-m" ] ; then
	SORTBY="mem"
    elif [ "${ARGI:0:7}" = "--apps=" ] ; then
        APPS="${ARGI#--apps=*}"
    else
        echo "Bad Argument: "$ARGI
        exit 1
    fi
done


#-------------------------------------------------------
#  Part 3: Make sure the pgrep utility is present
#-------------------------------------------------------
command -v pgrep
if [ $? != 0 ]; then
    echo "The required utity pgrep is not found. Exiting."
    exit 1
fi
    
#-------------------------------------------------------
#  Part 4: Form list of PIDs based on the app list
#-------------------------------------------------------
declare -a PIDS=()

# If apps are named explicitly, just use the list
if [ "${APPS}" != "" ] ; then
    IFS=', ' read -r -a apps_array <<< "$APPS"
    for app in "${apps_array[@]}"
    do
	PIDS+=($(pgrep $app))	
    done
# If no apps named explicitly, look for Antler and kids
else
    ANTLER_PIDS=($(pgrep pAntler))
    for ppid in "${ANTLER_PIDS[@]}"
    do
	PIDS+=($(pgrep -P $ppid))
    done
fi

if [ "${ANTLER}" = "yes" ]; then
    PIDS+=($(pgrep pAntler))
fi


# Check for and handle if no PIDs to watch
if [ -z "$PIDS" ]; then
    echo "No MOOS apps named or found. Exiting."
    exit 1
fi


#-------------------------------------------------------
#  Part 5: Determine the operating system
#-------------------------------------------------------
if [ -d "/proc" ]; then
    OS="linux"
fi

#-------------------------------------------------------
#  Part 6: Based on the OS, form cmdline args for top
#-------------------------------------------------------
ARGS=""
if [ "${OS}" = "osx" ]; then
    if [ "${SORTBY}" = "mem" ]; then
	ARGS+=" -o mem"
    else
	ARGS+=" -o cpu"
    fi
    ARGS+=" -stats pid,pgrp,ppid,cpu,mem,user,command"
    for pid in "${PIDS[@]}"
    do
	ARGS+=" -pid $pid"
    done
else
    if [ "${SORTBY}" = "mem" ]; then
	ARGS+=" -o %MEM"
    else
	ARGS+=" -o %CPU"
    fi
    PID_ARGS=""
    for pid in "${PIDS[@]}"
    do
	if [ "${PID_ARGS}" != "" ]; then
	    PID_ARGS+=","
	fi
	PID_ARGS+="$pid"
    done
    ARGS+=" -p "$PID_ARGS
fi
	    
if [ "${VERBOSE}" = "yes" ]; then
    echo "Operating System: " $OS
    echo "Command line args:" $ARGS
fi

#-------------------------------------------------------
top $ARGS
    
exit 0
