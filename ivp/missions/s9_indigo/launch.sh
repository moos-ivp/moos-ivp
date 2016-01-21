#!/bin/bash -e

TIME_WARP=1
for ARGI; do
    if [ "${ARGI}" = "--help" -o "${ARGI}" = "-h" ] ; then
	printf "%s [SWITCHES]                                       \n" $0
	printf "Switches:                                               \n" 
	printf "  --warp=WARP_VALUE                                     \n" 
	printf "  --help, -h                                            \n" 
	printf "Notes:                                                  \n"
	printf " (1) If one numerical argument given, it is interpreted \n"
	printf "     as being a warp value request.                     \n"
	printf "     ./launch.sh 12  same as:  ./launch.sh --warp=12    \n"
	exit 0;
    elif [ "${ARGI//[^0-9]/}" = "$ARGI" -a "$TIME_WARP" = 1 ]; then 
        TIME_WARP=$ARGI
    else 
	printf "Bad Argument: %s \n" $ARGI
	exit 0
    fi
done

#-------------------------------------------------------
#  Part 3: Launch the processes
#-------------------------------------------------------

printf "Launching ... (WARP=%s) \n" $TIME_WARP
pAntler indigo.moos --MOOSTimeWarp=$TIME_WARP >& /dev/null &
printf "Done Launching... \n"


#-------------------------------------------------------
#  Part 4: Exiting and/or killing the simulation
#-------------------------------------------------------

ANSWER="0"
while [ "${ANSWER}" != "1" -a "${ANSWER}" != "2" ]; do
    printf "Now what? (1) Exit script (2) Exit and Kill Simulation \n"
    printf "> "
    read ANSWER
done

# %1 matches the PID of the first job in the active jobs list,
# namely the pAntler job launched in Part 3.
if [ "${ANSWER}" = "2" ]; then
    printf "Killing all processes ... \n"
    kill %1
    printf "Done killing processes.   \n"
fi

exit 0
