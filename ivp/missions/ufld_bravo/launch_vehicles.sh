#!/bin/bash -e
#-------------------------------------------------------
#  Part 1: Check for and handle command-line arguments
#-------------------------------------------------------
TIME_WARP=1
AMT=1
SHORE="multicast_9"
for ARGI; do
    if [ "${ARGI}" = "--help" -o "${ARGI}" = "-h" ] ; then
	printf "%s [SWITCHES] [time_warp]   \n" $0
	printf " --help, -h                 \n"
	printf " --amt=N                    \n"
	printf " --shore=IPADDR             \n"
	printf " --shore=multicast_N        \n"
	exit 0;
    elif [ "${ARGI:0:6}" = "--amt=" ] ; then
        AMT="${ARGI#--amt=*}"
    elif [ "${ARGI:0:8}" = "--shore=" ] ; then
        SHORE="${ARGI#--shore=*}"
    elif [ "${ARGI//[^0-9]/}" = "$ARGI" -a "$TIME_WARP" = 1 ]; then 
        TIME_WARP=$ARGI
    else 
        echo "launch.sh Bad arg:" $ARGI " Exiting with code: 1"
        exit 1
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

if [ $AMT -gt 8 ] ; then
printf "Launching Ida....\n"
./launch_vehicle.sh --vname=ida $TIME_WARP --shore=$SHORE --index=9 --startpos="150,0" >& /dev/null &
fi

if [ $AMT -gt 10 ] ; then
printf "Launching Jake....\n"
./launch_vehicle.sh --vname=jake $TIME_WARP --shore=$SHORE --index=10 --startpos="160,0" >& /dev/null &
fi

if [ $AMT -gt 11 ] ; then
printf "Launching Karl....\n"
./launch_vehicle.sh --vname=karl $TIME_WARP --shore=$SHORE --index=11 --startpos="170,0" >& /dev/null &
fi

if [ $AMT -gt 12 ] ; then
printf "Launching Larry....\n"
./launch_vehicle.sh --vname=larry $TIME_WARP --shore=$SHORE --index=12 --startpos="180,0" >& /dev/null &
fi
