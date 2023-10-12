#!/bin/bash 
#----------------------------------------------------------
#  Script: mrecap.sh  (mission recap)  
#  Author: Michael Benjamin    
#  LastEd: Sep 2nd 2020          
#----------------------------------------------------------
#  Part 1: Set Exit actions and declare global var defaults
#----------------------------------------------------------  
DIR="./"
VERBOSE="no"

#-------------------------------------------------------
#  Part 2: Check for and handle command-line arguments
#-------------------------------------------------------
for ARGI; do
    echo "Arg:["$ARGI"]"
    if [ "${ARGI}" = "--help" -o "${ARGI}" = "-h" ]; then
	echo "mrecap.sh [OPTIONS]                                   "
	echo "  --help, -h         Show this help message           " 
	echo "  --verbose, -v      Show directories with no README  " 
	exit 0;
    elif [ "${ARGI}" = "--verbose" -o "${ARGI}" = "-v" ]; then
	VERBOSE="yes"
    else 
	echo "mrecap.sh: Bad Arg:" $ARGI
	exit 1
    fi
done

#-------------------------------------------------------
#  Part 3: Find and present the information
#-------------------------------------------------------
for file in *; do
   if [ -d $file ]; then
      cd $file; 
      if [ -e README ]; then
	  printf '%-28s' "$file"
	  head -n 1 README
      else
	  if [ ${VERBOSE} = "yes" ] ; then
	      printf '%-40s' "$file"
	      printf "n/a \n"
	  fi	  
      fi
      cd .. 
   fi
done
