#!/bin/bash -e


#-------------------------------------------------------
#  Part 1: Check for and handle command-line arguments
#-------------------------------------------------------
TIME_WARP=1
JUST_MAKE="no"
for ARGI; do
    if [ "${ARGI}" = "--help" -o "${ARGI}" = "-h" ] ; then
	printf "%s [SWITCHES] [time_warp]   \n" $0
	printf "  --help, -h         \n" 
	exit 0;
    elif [ "${ARGI//[^0-9]/}" = "$ARGI" -a "$TIME_WARP" = 1 ]; then 
        TIME_WARP=$ARGI
    else 
	printf "Bad Argument: %s \n" $ARGI
	exit 0
    fi
done

./launch.sh $TIME_WARP >& /dev/null &
sleep 5
uPokeDB targ_shoreside.moos DEPLOY_ALL=true MOOS_MANUAL_OVERRIDE_ALL=false
sleep 5

COND="\"((DB_UPTIME > 500) or (ENCOUNTER_TOTAL > 4))\""

while true; do 
    if uQueryDB targ_shoreside.moos --condition="ENCOUNTER_TOTAL > 1000" ; then 
	echo "Quitting...."
	ktm >& /dev/null
	exit 0
    fi
    sleep 30
    echo "continuing...."
done
