#!/bin/bash 
#--------------------------------------------------------
#  Script: bhvpath.sh
#  Author: Mike Benjamin
#  About:  Output a IVP_BEHAVIOR_DIRS path output one dir per line

AMT=20
for INDEX in `seq 1 $AMT`;
    do
      	PART=`echo $IVP_BEHAVIOR_DIRS | cut -d : -f $INDEX`
	if [ "${PART}" = "" ]; then
            exit 0
        fi
        echo "${INDEX}: $PART"
    done
}
