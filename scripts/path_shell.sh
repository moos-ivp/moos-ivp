#!/bin/bash 
#--------------------------------------------------------
#  Script: mypath.sh
#  Author: Mike Benjamin
#  About:  Output a shell path output one dir per line

AMT=75
for INDEX in `seq 1 $AMT`;
do
    PART=`echo $PATH | cut -d : -f $INDEX`
    if [ "${PART}" = "" ]; then
        exit 0
    fi
    echo "${INDEX}: $PART"
done
