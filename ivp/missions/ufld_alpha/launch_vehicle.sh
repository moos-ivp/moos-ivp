#!/bin/bash 
#-------------------------------------------------------
#  Part 1: Check for and handle command-line arguments
#-------------------------------------------------------
THIS_NAME=$(id -un)"@"$(hostname -s)
TIME_WARP=1
JUST_MAKE="no"
SHOREIP="localhost"
SHOREPORT="9300"
TERM_WAIT="yes"

for ARGI; do
    if [ "${ARGI}" = "--help" -o "${ARGI}" = "-h" ] ; then
	printf "%s [SWITCHES] [time_warp] \n" $0
	printf "  --just_make, -j                      \n" 
	printf "  --nowait, -n                         \n" 
	printf "  --shoreip=<ipaddr> default=localhost \n" 
	printf "  --shoreport=<port> default=9300      \n" 
	printf "  --help, -h                           \n" 
	exit 0;
    elif [ "${ARGI}" = "--nowait" -o "${ARGI}" = "-n" ] ; then
	TERM_WAIT="no"
    elif [ "${ARGI//[^0-9]/}" = "$ARGI" -a "$TIME_WARP" = 1 ]; then 
        TIME_WARP=$ARGI
    elif [ "${ARGI}" = "--just_build" -o "${ARGI}" = "-j" ] ; then
	JUST_MAKE="yes"
    elif [ "${ARGI:0:9}" = "--shoreip" ] ; then
        SHOREIP="${ARGI#--shoreip=*}"
    elif [ "${ARGI:0:11}" = "--shoreport" ] ; then
        SHOREIP="${ARGI#--shoreport=*}"
    else 
	printf "Bad Argument: %s \n" $ARGI
	exit 0
    fi
done

echo "Vehicle Name: " $THIS_NAME

#-------------------------------------------------------
#  Part 2: Create the .moos and .bhv files. 
#-------------------------------------------------------
GROUP="GROUP12"
VNAME=$THIS_NAME 
START_POS="0,0"  
LOITER_POS="x=0,y=-75"

nsplug meta_vehicle.moos targ_vehicle.moos -f WARP=$TIME_WARP \
    VNAME=$THIS_NAME          SHARE_LISTEN="9301"             \
    VPORT="9300"              START_POS=$START_POS            \
    GROUP="GROUP12"           LOITER_POS=$LOITER_POS          \
    SHOREPORT=$SHOREPORT      SHOREIP=$SHOREIP           

nsplug meta_vehicle.bhv targ_$VNAME.bhv -f VNAME=$VNAME       \
    START_POS=$START_POS LOITER_POS=$LOITER_POS       

if [ ${JUST_MAKE} = "yes" ] ; then
    exit 0
fi

#-------------------------------------------------------
#  Part 3: Launch the process(es)
#-------------------------------------------------------
printf "Launching $VNAME MOOS Community (WARP=%s) \n" $TIME_WARP
pAntler targ_vehicle.moos >& /dev/null &

if [ ${TERM_WAIT} != "yes" ] ; then
    exit 0
fi

#-------------------------------------------------------
#  Part 4: Exiting and/or killing the simulation
#-------------------------------------------------------
ANSWER="0"
while [ "${ANSWER}" != "1" -a "${ANSWER}" != "2" ]; do
    printf "Now what? (1) Exit script (2) Exit and Kill Simulation \n"
    printf "> "
    read ANSWER
done

# %1, %2, %3 matches the PID of the first three jobs in the active
# jobs list, namely the three pAntler jobs launched in Part 3.
if [ "${ANSWER}" = "2" ]; then
    printf "Killing all processes ... \n"
    kill %1 
    printf "Done killing processes.   \n"
fi


