#!/bin/bash 
#-------------------------------------------------------
#   Script: clean.sh                       
#-------------------------------------------------------
#  Part 1: Set global var defaults
#----------------------------------------------------------
VERBOSE=""

#-------------------------------------------------------
#  Part 2: Check for and handle command-line arguments
#-------------------------------------------------------
for ARGI; do
    if [ "${ARGI}" = "--help" -o "${ARGI}" = "-h" ] ; then
	echo "%s [SWITCHES]                       "
	echo "  --verbose                         " 
	echo "  --help, -h                        " 
	exit 0;	
    elif [ "${ARGI}" = "--verbose" -o "${ARGI}" = "-v" ] ; then
	VERBOSE="-v"
    else 
	printf "clean.sh Bad Arg:" $ARGI "Exit code 1"
	exit 1
    fi
done

#-------------------------------------------------------
#  Part 3: Do the cleaning!
#-------------------------------------------------------
rm -rf  $VERBOSE  MOOSLog_*  LOG_* 
rm -f   $VERBOSE  *~  targ_* *.moos++
rm -f   $VERBOSE  .LastOpenedMOOSLogDirectory
