#!/bin/bash -e

#-------------------------------------------------------
#  Part 1: Check for and handle command-line arguments
#-------------------------------------------------------
TIME_WARP=1
INHIBIT_GUI="no"
AMT=1000
for ARGI; do
    if [ "${ARGI}" = "--help" -o "${ARGI}" = "-h" ] ; then
	printf "%s [SWITCHES] [time_warp]             \n" $0
	printf "  --help, -h                          \n" 
	printf "  --no_gui, -n   (no pMarienViewer)   \n" 
	printf "  --amt=N        (total encounters)   \n" 
	exit 0;
    elif [ "${ARGI//[^0-9]/}" = "$ARGI" -a "$TIME_WARP" = 1 ]; then 
        TIME_WARP=$ARGI
    elif [ "${ARGI}" = "--no_gui" -o "${ARGI}" = "-n" ] ; then
	INHIBIT_GUI="yes"
    elif [ "${ARGI:0:6}" = "--amt=" ] ; then
        AMT="${ARGI#--amt=*}"
    else 
	printf "Bad Argument: %s \n" $ARGI
	exit 0
    fi
done

if [ "${INHIBIT_GUI}" = "yes" ] ; then
    ./launch.sh --no_gui $TIME_WARP >& /dev/null &
else
    ./launch.sh $TIME_WARP >& /dev/null &
fi
    
sleep 5
echo "Poking...." 
uPokeDB targ_shoreside.moos DEPLOY_ALL=true MOOS_MANUAL_OVERRIDE_ALL=false
sleep 5

# Run for at most 5 hours or at most $AMT encounters
#COND="\"((DB_UPTIME > 18000) or (ENCOUNTER_TOTAL > $AMT))\""

DONE="false"

while [ "${DONE}" = "false" ] ; do 
    if uQueryDB targ_shoreside.moos --condition="ENCOUNTER_TOTAL > $AMT" ; then 
	DONE="true"
    else
	echo "continuing...."
	sleep 15
    fi
done

echo "Quitting...."
ktm >& /dev/null
echo "Beginning pare...."
./pare.sh --rmo
echo "Pare complete."

RESULTS_DIR="results_"`date "+%Y_%m_%d_____%H_%M"`
mkdir $RESULTS_DIR
mv targ* *LOG* $RESULTS_DIR
