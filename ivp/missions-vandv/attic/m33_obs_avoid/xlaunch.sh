#!/bin/bash -e

#-------------------------------------------------------
#  Part 1: Check for and handle command-line arguments
#-------------------------------------------------------
TIME_WARP=1
JUST_MAKE="no"
NO_GUI=""

for ARGI; do
    if [ "${ARGI}" = "--help" -o "${ARGI}" = "-h" ] ; then
	echo "xlaunch.sh [SWITCHES] [time_warp]                  "
	echo "  --help, -h         Show this help message        " 
	echo "  --nogui, -n        Launch with no shoreside GUI  " 
	exit 0;
    elif [ "${ARGI}" = "--nogui" -o "${ARGI}" = "-n" ] ; then
	NO_GUI="--nogui"
    elif [ "${ARGI//[^0-9]/}" = "$ARGI" -a "$TIME_WARP" = 1 ]; then 
        TIME_WARP=$ARGI
    else 
	echo "Bad Argument: " $ARGI
	exit 0
    fi
done

./launch.sh $TIME_WARP $NO_GUI >& /dev/null &
sleep 4
uPokeDB targ_shoreside.moos DEPLOY_ALL=true MOOS_MANUAL_OVERRIDE_ALL=false
sleep 4

COND="((OB_TOTAL_ENCOUNTERS>20)or(DB_UPTIME>15000))"

while true; do 
    if uQueryDB targ_shoreside.moos --condition=$COND ; then 
	echo "Quitting...."
	ktm >& /dev/null
	break
    fi
    sleep 5
    echo "continuing...."
done

echo "================================================================="
echo "xlaunch.sh: Starting to TALLY . . . "
echo "================================================================="
./tally.sh

echo "================================================================="
echo "xlaunch.sh: Starting to PARE . . . "
echo "================================================================="
./pare.sh

