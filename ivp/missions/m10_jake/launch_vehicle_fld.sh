#!/bin/bash -e
#-------------------------------------------------------
#  Part 1: Check for and handle command-line arguments
#-------------------------------------------------------
TIME_WARP=1
JUST_MAKE="no"
VNAME="puddy"
for ARGI; do
    if [ "${ARGI}" = "--help" -o "${ARGI}" = "-h" ] ; then
	printf "%s [SWITCHES] [time_warp]   \n" $0
	printf "  --just_make, -j    \n" 
	printf "  --vname=VNAME      \n" 
	printf "  --help, -h         \n" 
	exit 0;
    elif [ "${ARGI:0:8}" = "--vname=" ] ; then
        VNAME="${ARGI#--vname=*}"
    elif [ "${ARGI//[^0-9]/}" = "$ARGI" -a "$TIME_WARP" = 1 ]; then 
        TIME_WARP=$ARGI
    elif [ "${ARGI}" = "--just_make" -o "${ARGI}" = "-j" ] ; then
	JUST_MAKE="yes"
    else 
        echo "launch_vehicle_fld.sh Bad arg:" $ARGI " Exiting with code: 1"
        exit 1
    fi
done

#-------------------------------------------------------
#  Part 2: Create the .moos and .bhv files. 
#-------------------------------------------------------
START_POS1="0,-30"  

nsplug meta_vehicle_fld.moos targ_$VNAME.moos -f WARP=$TIME_WARP  \
   VNAME=$VNAME       VTYPE=KAYAK                                 \
   VPORT="9001"       SHARE_LISTEN="9301"                         \

nsplug meta_vehicle.bhv targ_$VNAME.bhv -f VNAME=$VNAME           \
    START_POS=$START_POS1 

if [ ${JUST_MAKE} = "yes" ] ; then
    exit 0
fi

#-------------------------------------------------------
#  Part 3: Launch the processes
#-------------------------------------------------------
printf "Launching $VNAME MOOS Community (WARP=%s) \n" $TIME_WARP
pAntler targ_$VNAME.moos >& /dev/null &

uMAC targ_$VNAME.moos

kill %1 
printf "Done.   \n"


