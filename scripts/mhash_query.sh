#!/bin/bash 
#---------------------------------------------------------------
#   Script: mhash_query.sh
#   Author: Mike Benjamin
#   LastEd: May 2023
#-------------------------------------------------------------- 
#  Part 1: Define a convenience function for producing terminal
#          debugging/status output depending on the verbosity.
#-------------------------------------------------------------- 
vecho() { if [ "$VERBOSE" != "" ]; then echo "$ME: $1"; fi }

#---------------------------------------------------------------
#  Part 2: Set global var defaults
#---------------------------------------------------------------
ME=`basename "$0"`
DIR=$(pwd)
VERBOSE=""
SHOW=""
MHASH=""
MODE=""
TERSE=""

MHASH_FILE=""

rst=$(tput setaf 0)  # Reset
red=$(tput setaf 1)  # Red
grn=$(tput setaf 2)  # Green
blu=$(tput setaf 4)  # Blue
pur=$(tput setaf 5)  # Purple
cyn=$(tput setaf 6)  # Cyan

#---------------------------------------------------------------
#  Part 3: Check for and handle command-line arguments
#---------------------------------------------------------------
for ARGI; do
    if [ "${ARGI}" = "--help" -o "${ARGI}" = "-h" ]; then
	echo "$ME [OPTIONS] [file.mhash]                             "
	echo "                                                       "
	echo "Synopsis:                                              "
	echo "  $ME will examine the given .mhash file, or look for  "
	echo "  an .mhash file in the current directory. It supports "
	echo "  several query options, essentially verbatim  fields  "
	echo "  in the .mhash file or age queries.                   "
	echo "                                                       "
	echo "Options:                                               "
	echo "  --help, -h         Show this help message            "
	echo "  --verbose, -v      Verbose output                    "
	echo "  --terse, -t        Exclude newline char in output    "
	echo "                                                       "
	echo "  --mhash, -m        Return the mhash                  "
	echo "  --bhash, -b        Return the brief mhash (ADJ-NOUN) "
	echo "  --odo, -o          Return alog odometry              "
	echo "  --duration, -d     Return alog duration              "
	echo "  --name, -n         Return mhash vehicle name         "
	echo "  --utc, -u          Return alog UTC start time        "
	echo "  --xhash, -x        Return xhash component            "
	echo "  --ages, -as        Return trunc age in seconds       "
	echo "  --ageh, -ah        Return trunc age in hours         "
	echo "  --aged, -ad        Return trunc age in days          "
	echo "  --all, -a          Return the full mhash string      "
	echo "                                                       "
	echo "Return Value:                                          "
	echo "  0 upon success                                       "
	echo "  1 upon failure                                       "
	exit 0
    elif [ "${ARGI}" = "--verbose" -o "${ARGI}" = "-v" ]; then
	VERBOSE="-v"
    elif [[ "${ARGI}" = *".mhash" ]]; then 
	MHASH_FILE=$ARGI
    elif [ "${ARGI}" = "--terse" -o "${ARGI}" = "-t" ]; then
	TERSE="-n"
    elif [ "${ARGI}" = "--mhash" -o "${ARGI}" = "-m" ]; then
	SHOW="--mhash"
    elif [ "${ARGI}" = "--bhash" -o "${ARGI}" = "-b" ]; then
	SHOW="--bhash"
    elif [ "${ARGI}" = "--odo" -o "${ARGI}" = "-o" ]; then
	SHOW="--odo"
    elif [ "${ARGI}" = "--duration" -o "${ARGI}" = "-d" ]; then
	SHOW="--duration"
    elif [ "${ARGI}" = "--name" -o "${ARGI}" = "-n" ]; then
	SHOW="--name"
    elif [ "${ARGI}" = "--utc" -o "${ARGI}" = "-u" ]; then
	SHOW="--utc"
    elif [ "${ARGI}" = "--xhash" -o "${ARGI}" = "-x" ]; then
	SHOW="--xhash"
    elif [ "${ARGI}" = "--ages" -o "${ARGI}" = "-as" ]; then
	SHOW="--ages"
    elif [ "${ARGI}" = "--ageh" -o "${ARGI}" = "-ah" ]; then
	SHOW="--ageh"
    elif [ "${ARGI}" = "--aged" -o "${ARGI}" = "-ad" ]; then
	SHOW="--aged"
    elif [ "${ARGI}" = "--all" -o "${ARGI}" = "-a" ]; then
	SHOW="--all"
    else
        echo "$ME: Bad arg:" $ARGI "Exit Code 1."
        exit 1
    fi
done


#---------------------------------------------------------------
#  Part 4: If MHASH file was given, check that it does exist.
#---------------------------------------------------------------
if [ "${MHASH_FILE}" != "" ]; then
    if [ ! -e $MHASH_FILE ]; then
	vecho "$red MHASH_FILE:["$MHASH_FILE"] does not exist. $rst"
	exit 1
    fi
    vecho "MHASH_FILE:["$MHASH_FILE"] confirmed to exist"
fi


#---------------------------------------------------------------
#  Part 5: If MHASH file not given, look for one
#---------------------------------------------------------------
if [ "${MHASH_FILE}" = "" ]; then
    for file in *; do
	if [[ $file == *.alog ]]; then		
	    filename=$(basename -- "$file")
	    extension="${filename##*.}"
	    filename="${filename%.*}"
	    mhash_file="${filename}.mhash"	    
	    if [ -e $mhash_file ]; then
		MHASH_FILE=$mhash_file
		vecho "MHASH_FILE found:[${MHASH_FILE}]"
	    fi
	fi
    done
fi

if [ "${MHASH_FILE}" = "" ]; then
    vecho "$red No MHASH file provided or found. Exit code 1. $rst"
    exit 1
fi

#---------------------------------------------------------------
#  Part 5: Show the component, or just exit 0 if checking the
#          existence of an .mhash file
#---------------------------------------------------------------
VAL="${MHASH_FILE}"
if [ "${SHOW}" = "--mhash" ]; then
    VAL=`head -1 ${MHASH_FILE} | cut -d"," -f1 | cut -d"=" -f2`
elif [ "${SHOW}" = "--bhash" ]; then
    JUST_MHASH=`head -1 ${MHASH_FILE} | cut -d"," -f1 | cut -d"=" -f2`
    ONE=`echo $JUST_MHASH | cut -d"-" -f3`
    TWO=`echo $JUST_MHASH | cut -d"-" -f4`
    VAL="${ONE}-${TWO}"
elif [ "${SHOW}" = "--odo" ]; then
    VAL=`head -1 ${MHASH_FILE} | cut -d"," -f2 | cut -d"=" -f2`
    VAL=`printf "%.0f\n" $VAL`
elif [ "${SHOW}" = "--duration" ]; then
    VAL=`head -1 ${MHASH_FILE} | cut -d"," -f3 | cut -d"=" -f2`
    VAL=`printf "%.0f\n" $VAL`
elif [ "${SHOW}" = "--name" ]; then
    VAL=`head -1 ${MHASH_FILE} | cut -d"," -f4 | cut -d"=" -f2`
elif [ "${SHOW}" = "--utc"  ]; then
    VAL=`head -1 ${MHASH_FILE} | cut -d"," -f5 | cut -d"=" -f2`
    VAL=`printf "%.0f\n" $VAL`
elif [ "${SHOW}" = "--xhash" ]; then
    VAL=`head -1 ${MHASH_FILE} | cut -d"," -f6 | cut -d"=" -f2`
elif [ "${SHOW}" = "--all" ]; then
    VAL=`head -1 ${MHASH_FILE}`
elif [ "${SHOW}" = "--ages" ]; then
    UTC=`head -1 ${MHASH_FILE} | cut -d"," -f5 | cut -d"=" -f2`
    NOW=`date +%s`
    VAL=$((NOW-UTC))
elif [ "${SHOW}" = "--ageh" ]; then
    UTC=`head -1 ${MHASH_FILE} | cut -d"," -f5 | cut -d"=" -f2`
    NOW=`date +%s`
    SEC=$((NOW-UTC))
    VAL=$((SEC/3600))
elif [ "${SHOW}" = "--aged" ]; then
    UTC=`head -1 ${MHASH_FILE} | cut -d"," -f5 | cut -d"=" -f2`
    NOW=`date +%s`
    SEC=$((NOW-UTC))
    VAL=$((SEC/86400))
fi



echo $TERSE $VAL
exit 0
