#!/bin/bash -e

#-------------------------------------------------------
#  Part 1: Check for and handle command-line arguments
#-------------------------------------------------------
TIME_WARP=1
JUST_MAKE="no"
NO_GUI=""

for ARGI; do
    if [ "${ARGI}" = "--help" -o "${ARGI}" = "-h" ] ; then
	printf "%s [SWITCHES] [time_warp]                          \n" $0
	printf "  --help, -h         Show this help message        \n" 
	printf "  --nogui, -n        Launch with no shoreside GUI  \n" 
	exit 0;
    elif [ "${ARGI}" = "--nogui" -o "${ARGI}" = "-n" ] ; then
	NO_GUI="--nogui"
    elif [ "${ARGI//[^0-9]/}" = "$ARGI" -a "$TIME_WARP" = 1 ]; then 
        TIME_WARP=$ARGI
    else 
	printf "Bad Argument: %s \n" $ARGI
	exit 0
    fi
done

./launch.sh $TIME_WARP $NO_GUI >& /dev/null &
sleep 5
uPokeDB targ_shoreside.moos DEPLOY_ALL=true MOOS_MANUAL_OVERRIDE_ALL=false
sleep 5

#COND="\"((DB_UPTIME>500)or(ENCOUNTER_TOTAL>20))\""

while true; do 
    if uQueryDB targ_shoreside.moos --condition="ENCOUNTER_TOTAL>200" ; then 
	echo "Quitting...."
	ktm >& /dev/null
	break
    fi
    sleep 5
    echo "continuing...."
done

echo "================================================================="
echo "xlaunch.sh: Starting to PARE . . . "
echo "================================================================="
./pare.sh

echo "================================================================="
echo "xlaunch.sh: Starting to SPLIT . . . "
echo "================================================================="
./split.sh

echo "================================================================="
echo "xlaunch.sh: Starting to TALLY . . . "
echo "================================================================="
./tally.sh
