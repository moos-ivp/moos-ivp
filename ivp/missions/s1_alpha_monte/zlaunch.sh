#!/bin/bash -e
#----------------------------------------------------------
#  Script: zlaunch.sh
#  Author: Michael Benjamin
#  LastEd: Dec 11th 2021
#----------------------------------------------------------
#  Part 1: Set global var defaults
#----------------------------------------------------------
ME=`basename "$0"`
TIME_WARP=1
VERBOSE="no"
FLOW_DOWN_ARGS=""
EACH=1

#----------------------------------------------------------
#  Part 2: Check for and handle command-line arguments
#----------------------------------------------------------
for ARGI; do
    if [ "${ARGI}" = "--help" -o "${ARGI}" = "-h" ]; then
	echo "$ME [SWITCHES] [time_warp]                      " 
	echo "  --help, -h      Show this help message               " 
        echo "  --verbose, -v   Enable verbose mode                  "
	echo "  --each=N        Run each sim N times. Default is 1   " 
	echo "  --res, -r       Tell xlaunch to generate report      " 
	echo "  --send, -s      Tell xlaunch to gen and send report  " 
	echo "  --silent        Run silently, no iSay                " 
	exit 0;
    elif [ "${ARGI//[^0-9]/}" = "$ARGI" -a "$TIME_WARP" = 1 ]; then 
        TIME_WARP=$ARGI
    elif [ "${ARGI}" = "--verbose" -o "${ARGI}" = "-v" ]; then
	VERBOSE="yes"
    elif [ "${ARGI:0:7}" = "--each=" ]; then
        EACH="${ARGI#--each=*}"
    elif [ "${ARGI}" = "--res" -o "${ARGI}" = "-r" ]; then
        FLOW_DOWN_ARGS+="${ARGI} "
    elif [ "${ARGI}" = "--send" -o "${ARGI}" = "-s" ]; then
        FLOW_DOWN_ARGS+="${ARGI} "
    elif [ "${ARGI}" = "--silent" ]; then
        FLOW_DOWN_ARGS+="${ARGI} "
    else 
        echo "$ME Bad arg:" $ARGI "Exiting with code: 1"
        exit 1
    fi
done

if [ ${VERBOSE} = "no" ]; then
    FLOW_DOWN_ARGS+=" --quiet "
fi

FLOW_DOWN_ARGS+="${TIME_WARP} --com=alpha "
echo "zlaunch.sh FLOW_DOWN_ARGS:[$FLOW_DOWN_ARGS]"

TEST_COUNT=0
for ((i=0; i<2; ++i)); do 
    STEP=0.2
    DELTA=$(echo $STEP*$i | bc) 
    for ((j=0; j<$EACH; ++j)); do
	((TEST_COUNT=TEST_COUNT+1))
	echo "Test Number: " $TEST_COUNT
	SPD=$(echo 3-$DELTA | bc)
	xlaunch.sh $FLOW_DOWN_ARGS --spd=$SPD --nogui 
    done
done

echo "done"
exit 0

# Convert raw results, to a file with averages over all experiments
if [ -f "results.log" ]; then
    alogavg results.log > results.dat
fi

# Build a plot using matlab
if [ -f "results.dat" ]; then
    matlab plotx.m -nodisplay -nosplash -r "plotx(\"results.dat\", \"plot.png\")"
fi

