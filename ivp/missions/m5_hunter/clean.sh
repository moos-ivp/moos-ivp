#!/bin/bash 
#----------------------------------------------------------
#  Part 1: Declare global var defaults
#----------------------------------------------------------
VERBOSE=""
DRY_RUN="no"

#-------------------------------------------------------
#  Part 2: Check for and handle command-line arguments
#-------------------------------------------------------
for ARGI; do
    if [ "${ARGI}" = "--help" -o "${ARGI}" = "-h" ] ; then
	echo "clean.sh [SWITCHES]                "
	echo "  --verbose                        " 
	echo "  --help, -h                       " 
	echo "  --dryrun, -d                     " 
	exit 0;	
    elif [ "${ARGI}" = "--verbose" -o "${ARGI}" = "-v" ] ; then
	VERBOSE="-v"
    elif [ "${ARGI}" = "--dry_run" -o "${ARGI}" = "-d" ] ; then
	DRY_RUN="yes"
	VERBOSE="-v"
    else 
	echo "clean.sh: Bad Arg:" $ARGI
	exit 0
    fi
done

#-------------------------------------------------------
#  Part 2: Do the cleaning!
#-------------------------------------------------------
if [ "${VERBOSE}" = "-v" ]; then
    echo "Cleaning: $PWD"
fi

FILES="MOOSLog_*  LOG_* LOG_* XLOG_* "
FILES+="*~  targ_* *.moos++ .tmp* "
FILES+=".LastOpenedMOOSLogDirectory"

if [ "${DRY_RUN}" = "no" ]; then
    rm -rf $VERBOSE $FILES
else
    echo $FILES
fi
