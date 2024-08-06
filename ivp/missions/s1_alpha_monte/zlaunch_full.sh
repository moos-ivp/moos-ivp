#!/bin/bash -e
TIME_WARP=1
EACH=1

for ARGI; do
    if [ "${ARGI//[^0-9]/}" = "$ARGI" -a "$TIME_WARP" = 1 ]; then 
        TIME_WARP=$ARGI
    elif [ "${ARGI:0:7}" = "--each=" ]; then
        EACH="${ARGI#--each=*}"
    fi
done


STEP=0.5
for ((i=0; i<10; ++i)); do 
    DELTA=$(echo $STEP*$i | bc) 
    for ((j=0; j<$EACH; ++j)); do
	SPD=$(echo 1+$DELTA | bc)
	echo "xlaunch.sh ${TIME_WARP} --com=alpha --spd=$SPD --nogui"
	xlaunch.sh ${TIME_WARP} --com=alpha --spd=$SPD --nogui 
    done
done

exit 0

# Convert raw results, to a file with averages over all experiments
if [ -f "results.txt" ]; then
    alogavg --file=results.txt > results.dat
fi

# Build a plot using matlab
if [ -f "results.dat" ]; then
    matlab plotx.m -nodisplay -nosplash -r "plotx(\"results.dat\", \"plot.png\")"
fi

