#!/bin/bash 
#-------------------------------------------------------
#  Part 1: Check for and handle command-line arguments
#-------------------------------------------------------
TIME_WARP=1
AMT=1
SHORE="multicast_9"
for ARGI; do
    if [ "${ARGI}" = "--help" -o "${ARGI}" = "-h" ] ; then
	printf "%s [time_warp]   \n" $0
	exit 0;
    elif [ "${ARGI:0:6}" = "--amt=" ] ; then
        AMT="${ARGI#--amt=*}"
    elif [ "${ARGI:0:8}" = "--shore=" ] ; then
        SHORE="${ARGI#--shore=*}"
    elif [ "${ARGI//[^0-9]/}" = "$ARGI" -a "$TIME_WARP" = 1 ]; then 
        TIME_WARP=$ARGI
    else 
	printf "Bad Argument: %s \n" $ARGI
	exit 0
    fi
done

#-------------------------------------------------------
#  Launch the vehicles
#-------------------------------------------------------

printf "Launching Archie....\n"
./launch_vehicle.sh --vname=archie  $TIME_WARP --shore=$SHORE --index=1 --startpos="10,0" >& /dev/null &

if [ $AMT -gt 1 ] ; then
printf "Launching Betty.... \n"
./launch_vehicle.sh --vname=betty   $TIME_WARP --shore=$SHORE --index=2 --startpos="30,0" >& /dev/null &
fi

if [ $AMT -gt 2 ] ; then
printf "Launching Charlie....\n"
./launch_vehicle.sh --vname=charlie $TIME_WARP --shore=$SHORE --index=3 --startpos="50,0" >& /dev/null &
fi

if [ $AMT -gt 3 ] ; then
printf "Launching Davis....\n"
./launch_vehicle.sh --vname=davis $TIME_WARP --shore=$SHORE --index=4 --startpos="70,0" >& /dev/null &
fi

if [ $AMT -gt 4 ] ; then
printf "Launching Ernie....\n"
./launch_vehicle.sh --vname=ernie $TIME_WARP --shore=$SHORE --index=5 --startpos="90,0" >& /dev/null &
fi

if [ $AMT -gt 5 ] ; then
printf "Launching Frank....\n"
./launch_vehicle.sh --vname=frank $TIME_WARP --shore=$SHORE --index=6 --startpos="100,0" >& /dev/null &
fi

if [ $AMT -gt 6 ] ; then
printf "Launching George....\n"
./launch_vehicle.sh --vname=george $TIME_WARP --shore=$SHORE --index=7 --startpos="120,0" >& /dev/null &
fi

if [ $AMT -gt 7 ] ; then
printf "Launching Henry....\n"
./launch_vehicle.sh --vname=henry $TIME_WARP --shore=$SHORE --index=8 --startpos="140,0" >& /dev/null &
fi
