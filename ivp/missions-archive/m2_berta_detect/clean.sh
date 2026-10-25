#!/bin/bash 
#--------------------------------------------------------------
#   Script: clean.sh                                    
#   Author: Michael Benjamin  
#     Date: June 2020     
#----------------------------------------------------------
#  Part 1: Declare global var defaults
#----------------------------------------------------------
VERBOSE=""
POST_MISSION_FILES="no"

#-------------------------------------------------------
#  Part 2: Check for and handle command-line arguments
#-------------------------------------------------------
for ARGI; do
    if [ "${ARGI}" = "--help" -o "${ARGI}" = "-h" ] ; then
	echo "clean.sh [SWITCHES]           "
	echo "  --help, -h                  " 
	echo "  --verbose       Verbose output during rm  " 
	echo "  --pmf           Clean post-misison files  " 
	exit 0;	
    elif [ "${ARGI}" = "--verbose" -o "${ARGI}" = "-v" ] ; then
	VERBOSE="-v"
    elif [ "${ARGI}" = "--pmf" ] ; then
	POST_MISSION_FILES="yes"
    else 
	echo "clean.sh: Bad Arg:" $ARGI
	exit 1
    fi
done

#-------------------------------------------------------
#  Part 2: Do the cleaning!
#-------------------------------------------------------
if [ "${VERBOSE}" = "-v" ]; then
    echo "Cleaning: $PWD"
fi

rm -rf  $VERBOSE   MOOSLog_*  XLOG_* LOG_* 
rm -f   $VERBOSE   *~  *.moos++ targ_*
rm -f   $VERBOSE   .LastOpenedMOOSLogDirectory

if [ "${POST_MISSION_FILES}" = "yes" ]; then
    rm -rf $VERBOSE *alvtmp 
    rm -rf $VERBOSE *pared.alog
    rm -rf $VERBOSE*.png *.pdf 
    rm -rf *.log
    rm -rf *_alog_files
fi


