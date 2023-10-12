#!/bin/bash 
#---------------------------------------------------------------
#   Script: mhash_unpack.sh
#   Author: Mike Benjamin
#   LastEd: June 2023
#---------------------------------------------------------------
#  Part 1: Set global var defaults
#---------------------------------------------------------------
ME=`basename "$0"`

#---------------------------------------------------------------
#  Part 2: Check for and handle command-line arguments
#---------------------------------------------------------------
for ARGI; do
    if [ "${ARGI}" = "--help" -o "${ARGI}" = "-h" ]; then
	echo "$ME [OPTIONS]                                 "
	echo "                                              "
	echo "Synopsis:                                     "
	echo "  $ME will look for all .tgz files in the     "
	echo "  current directory and will unpack them into "
	echo "  original contents.                          "
	echo "                                              "
	echo "Options:                                      "
	echo "  --help, -h         Show this help message   "
	exit 0
    else
        echo "$ME: Bad arg:" $ARGI "Exit Code 1."
        exit 1
    fi
done

#---------------------------------------------------------------
#  Part 3: Unpack the .tgz files
#---------------------------------------------------------------

for file in *; do
    if [[ $file == *.tgz ]]; then
       tar xvfz $file
    fi
done
