#!/bin/bash 
#--------------------------------------------------------------
#   Script: clean.sh                                    
#   Author: Michael Benjamin  
#     Date: June 2020     

VERBOSE=""

#-------------------------------------------------------
#  Part 1: Check for and handle command-line arguments
#-------------------------------------------------------
for ARGI; do
    if [ "${ARGI}" = "--help" -o "${ARGI}" = "-h" ]; then
	echo "clean.sh [SWITCHES]       "
	echo "   --verbose, -v          "
	echo "   --help, -h             "
    elif [ "${ARGI}" = "--verbose" -o "${ARGI}" = "-v" ]; then
	VERBOSE="-v"
    else
	echo "clean.sh: Bad Arg:" $ARGI
	exit 1
    fi
done

#-------------------------------------------------------
#  Part 2: Do the cleaning!
#-------------------------------------------------------

rm -rf  $VERBOSE   LOG_*
rm -f   $VERBOSE   *~
rm -f   $VERBOSE   targ_*
rm -f   $VERBOSE   .LastOpenedMOOSLogDirectory
