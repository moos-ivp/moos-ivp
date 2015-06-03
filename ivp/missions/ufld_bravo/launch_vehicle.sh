#!/bin/bash -e
#-------------------------------------------------------
#  Part 1: Check for and handle command-line arguments
#-------------------------------------------------------
TIME_WARP=1
JUST_MAKE="no"
VNAME="archie"
INDEX="1"
SHORE="multicast_9"
ORDER="normal"
SPEED="1.8"
START_POS="0,0"  
DELAY_TIME=0
for ARGI; do
    if [ "${ARGI}" = "--help" -o "${ARGI}" = "-h" ] ; then
	printf "%s [SWITCHES] [time_warp]   \n" $0
	printf "  --just_make, -j    \n" 
	printf "  --vname=VNAME      \n" 
	printf "  --index=INDEX      \n" 
	printf "  --help, -h         \n" 
	exit 0;
    elif [ "${ARGI//[^0-9]/}" = "$ARGI" -a "$TIME_WARP" = 1 ]; then 
        TIME_WARP=$ARGI
    elif [ "${ARGI:0:8}" = "--vname=" ] ; then
        VNAME="${ARGI#--vname=*}"
    elif [ "${ARGI:0:8}" = "--index=" ] ; then
        INDEX="${ARGI#--index=*}"
    elif [ "${ARGI:0:11}" = "--startpos=" ] ; then
        START_POS="${ARGI#--startpos=*}"
    elif [ "${ARGI:0:8}" = "--shore=" ] ; then
        SHORE="${ARGI#--shore=*}"
    elif [ "${ARGI}" = "--just_build" -o "${ARGI}" = "-j" ] ; then
	JUST_MAKE="yes"
    else 
	printf "Bad Argument: %s \n" $ARGI
	exit 0
    fi
done

#-------------------------------------------------------
#  Part 2: Create the .moos and .bhv files. 
#-------------------------------------------------------
# What is nsplug? Type "nsplug --help" or "nsplug --manual"

nsplug meta_vehicle.moos targ_$VNAME.moos -f WARP=$TIME_WARP  VTYPE=UUV \
   VNAME=$VNAME       START_POS=$START_POS        SHORE=$SHORE          \
   VPORT="900"$INDEX  SHARE_LISTEN="930"$INDEX   

nsplug meta_vehicle.bhv targ_$VNAME.bhv -f VNAME=$VNAME START_POS=$START_POS \
   ORDER=$ORDER   SPEED=$SPEED DELAY_TIME=$DELAY_TIME

if [ ${JUST_MAKE} = "yes" ] ; then
    exit 0
fi

#-------------------------------------------------------
#  Part 3: Launch the processes
#-------------------------------------------------------
printf "Launching $VNAME MOOS Community (WARP=%s) \n" $TIME_WARP
pAntler targ_$VNAME.moos >& /dev/null &
sleep .25
printf "Done \n"

uMAC targ_$VNAME.moos

printf "Killing all processes ... \n"
kill %1 
printf "Done killing processes.   \n"


