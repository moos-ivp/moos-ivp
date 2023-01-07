#!/bin/bash 
VERBOSE=""

#-------------------------------------------------------
#  Part 1: Check for and handle command-line arguments
#-------------------------------------------------------
for ARGI; do
    if [ "${ARGI}" = "--help" -o "${ARGI}" = "-h" ] ; then
	echo "clean.sh [SWITCHES]         "
	echo "  --verbose                 " 
	echo "  --help, -h                " 
	exit 0;	
    elif [ "${ARGI}" = "--verbose" -o "${ARGI}" = "-v" ] ; then
	VERBOSE="-v"
    else 
	echo "Bad Arg:" $ARGI
	exit 1
    fi
done

#-------------------------------------------------------
#  Part 2: Do the cleaning!
#-------------------------------------------------------

rm -rf  $VERBOSE   MOOSLog_*  LOG_* XLOG_*
rm -f   $VERBOSE   *~  targ_* *.moos++
rm -f   $VERBOSE   .LastOpenedMOOSLogDirectory
