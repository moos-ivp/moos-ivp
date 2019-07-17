#!/bin/bash
#--------------------------------------------------------------
#    Script: appmem.sh
#    Author: Michael Benjamin
#      Date: May 4th 2019
#  Synopsis: This script determines the amount of memory currently
#            used by the given process ID (PID)
#--------------------------------------------------------------
 
#--------------------------------------------------------------
#  Part 1: Initialize global vars
#--------------------------------------------------------------

# Make sure we augment the path to find the svn executable
OS="osx"
PID=""
VERBOSE="no"

#--------------------------------------------------------------
#  Part 2: Handle Command Line args
#--------------------------------------------------------------
for ARGI; do
    if [ "${ARGI}" = "--help" -o "${ARGI}" = "-h" ] ; then
	echo "Usage:                                                  "
	echo "  appmem.sh [SWITCHES]                                  "
	echo "                                                        "
	echo "Synopsis:                                               "
	echo "  This script determines the amount of memory currently "
	echo "  used by the given process ID (PID). It uses the \"ps\""
	echo "  with restricted output to be quick and limit the      "
	echo "  parsing needed by the user.                           "
	echo "                                                        "
	echo "Switches:                                               "
	echo "  --help, -h            Show this help message          " 
        echo "  --verbose, -v         Produce extra verbosity         "
	echo "  --pid=<PID>           Specify the process by PID      " 
	echo "                                                        "
	exit 0
    elif [ "${ARGI}" = "--verbose" -o "${ARGI}" = "-v" ] ; then
        VERBOSE="yes"
    elif [ "${ARGI:0:6}" = "--pid=" ] ; then
        PID="${ARGI#--pid=*}"
    else
	echo "Unrecognized argument. Run with --help                  "
	exit 1
    fi
done

#--------------------------------------------------------------
#  Part 3: Error check
#--------------------------------------------------------------
if [ "${PID}" = "" ]; then
    echo "A PID must be given. Run --help for more info."
    exit 2
fi

if [ "${VERBOSE}" = "yes" ]; then
    echo "PID:["$PID"]"
fi

#--------------------------------------------------------------
#  Part 4: Determine if this is Linux or OSX
#--------------------------------------------------------------
if [ -d "/proc" ]; then
    OS="linux"
fi

if [ "${VERBOSE}" = "yes" ]; then
    echo "Operating System:" $OS
fi
    
#--------------------------------------------------------------
#  Part 5: Get the PS results 
#--------------------------------------------------------------
res=""

#  Part 5A: Handle the OSX case
if [ "${OS}" = "osx" ]; then
    RES=`ps -p $PID -o rss=`

#  Part 5A: Handle the Linux case (appears that ps works the same)
else
    RES=`ps -p $PID -o rss=`
fi

#--------------------------------------------------------------
#  Part 6: Examine and report the results
#--------------------------------------------------------------
if [ "${RES}" = "" ]; then
    echo "0"
    exit 4
fi

echo $RES
exit 0
