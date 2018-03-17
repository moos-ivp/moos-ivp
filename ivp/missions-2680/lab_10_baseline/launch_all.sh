#!/bin/bash 

#-------------------------------------------------------
#  Part 1: Check for and handle command-line arguments
#-------------------------------------------------------
TIME_WARP=1
JUST_MAKE="no"
for ARGI; do
    if [ "${ARGI}" = "--help" -o "${ARGI}" = "-h" ] ; then
	printf "%s [SWITCHES] [time_warp]   \n" $0
	printf "  --just_make, -j    \n" 
	printf "  --help, -h         \n" 
	exit 0;
    elif [ "${ARGI//[^0-9]/}" = "$ARGI" -a "$TIME_WARP" = 1 ]; then 
        TIME_WARP=$ARGI
    elif [ "${ARGI}" = "--just_build" -o "${ARGI}" = "-j" ] ; then
        JUST_MAKE="yes"
    else 
	printf "Bad Argument: %s \n" $ARGI
	exit 0
    fi
done

echo "Launching Henry...."
./launch_vehicle.sh --vname=henry --mport=9001 --lport=9201 $TIME_WARP

echo "Launching Gilda...."
./launch_vehicle.sh --vname=gilda --mport=9002 --lport=9202 $TIME_WARP

echo "Launching Shoreside...."
./launch_shoreside.sh $TIME_WARP


