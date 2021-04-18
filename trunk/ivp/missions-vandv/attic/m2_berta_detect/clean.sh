#!/bin/bash 

VERBOSE=""

#-------------------------------------------------------
#  Part 1: Check for and handle command-line arguments
#-------------------------------------------------------
for ARGI; do
    if [ "${ARGI}" = "--help" -o "${ARGI}" = "-h" ] ; then
	echo "clean.sh [SWITCHES]                 "
	echo "  --verbose                         " 
	echo "  --help, -h                        " 
	exit 0;	
    elif [ "${ARGI}" = "--verbose" -o "${ARGI}" = "-v" ] ; then
	VERBOSE="-v"
    else 
	echo "Bad Argument: " $ARGI
	exit 0
    fi
done

#-------------------------------------------------------
#  Part 2: Do the cleaning!
#-------------------------------------------------------

rm -rf  $VERBOSE   MOOSLog_*  LOG_* LLOG_* XLOG_* *.ps
rm -f   $VERBOSE   *~  targ_* *.moos++ .tmp*
rm -f   $VERBOSE   .LastOpenedMOOSLogDirectory
rm -rf  *alvtmp
rm -rf  *pared.alog
