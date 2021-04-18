#!/bin/bash
#--------------------------------------------------------------
#    Script: info_date.sh
#    Author: Michael Benjamin
#      Date: September 2020
#  Synopsis: Gather date info for either a multi-line output
#            for a results file, or in a single-line output
#            suitable for using in a file name.
#--------------------------------------------------------------
#  Part 1: Set global vars
#--------------------------------------------------------------
TERSE="no"

#--------------------------------------------------------------
#  Part 2: Handle Command Line args
#--------------------------------------------------------------
for ARGI; do
    if [ "${ARGI}" = "--help" -o "${ARGI}" = "-h" ]; then
	echo "info_date.sh [OPTIONS]                     "
	echo "  --help, -h       Show this help message  "
	echo "  --terse, -t      Produce terse output    "
	exit 0
    elif [ "${ARGI}" = "--terse" -o "${ARGI}" = "-t" ]; then
        TERSE="yes"
    else
	echo "info_date.sh: Bad Arg: "$ARGI
	exit 1
    fi
done

#--------------------------------------------------------------
#  Part 3: Get the date and time info 
#--------------------------------------------------------------
YR=$(date +%Y)
MON=`date +%m`
DAY=`date +%d`
HR=`date +%H`
MIN=`date +%M`
SEC=`date +%S`

#--------------------------------------------------------------
#  Part 4: output results
#--------------------------------------------------------------
if [ "${TERSE}" = "yes" ]; then
    echo $YR"_"$MON"_"$DAY"___"$HR"_"$MIN"_"$SEC
    exit 0
fi

echo "year=$YR" 
echo "month=$MON"
echo "day=$DAY" 
echo "hour=$HR" 
echo "min=$MIN" 
echo "sec=$SEC" 
