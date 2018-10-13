#!/bin/bash -e
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

#-------------------------------------------------------
#  Part 2: Create the .moos and .bhv files. 
#-------------------------------------------------------
VNAME1="henry"         # The first vehicle Community
VNAME2="gilda"         # The second vehicle Community
START_POS1="50,-50"    # Vehicle 1 Behavior configurations
START_POS2="80,-125"   # Vehicle 2 Behavior configurations
SHORE_LISTEN="9300"

nsplug meta_vehicle.moos targ_henry.moos -f WARP=$TIME_WARP \
   VNAME=$VNAME1      START_POS=$START_POS1                 \
   VPORT="9001"       SHARE_LISTEN="9301"                   \
   VTYPE="kayak"      SHORE_LISTEN=$SHORE_LISTEN            \
   KNOWS_CONTACTS=1

nsplug meta_vehicle.moos targ_gilda.moos -f WARP=$TIME_WARP \
   VNAME=$VNAME2      START_POS=$START_POS2                 \
   VPORT="9002"       SHARE_LISTEN="9302"                   \
   VTYPE="kayak"      SHORE_LISTEN=$SHORE_LISTEN

nsplug meta_shoreside.moos targ_shoreside.moos -f WARP=$TIME_WARP \
   SNAME="shoreside"  SHARE_LISTEN=$SHORE_LISTEN                  \
   SPORT="9000"     

nsplug meta_henry.bhv targ_henry.bhv -f VNAME=$VNAME1  \
    OVNAME=$VNAME2 START_POS=$START_POS2 

nsplug meta_gilda.bhv targ_gilda.bhv -f VNAME=$VNAME2  \
    OVNAME=$VNAME1 START_POS=$START_POS1 

if [ ${JUST_MAKE} = "yes" ] ; then
    exit 0
fi

#-------------------------------------------------------
#  Part 3: Launch the processes
#-------------------------------------------------------
printf "Launching $VNAME1 MOOS Community (WARP=%s) \n" $TIME_WARP
pAntler targ_henry.moos >& /dev/null &
sleep 0.25
printf "Launching $VNAME2 MOOS Community (WARP=%s) \n" $TIME_WARP
pAntler targ_gilda.moos >& /dev/null &
sleep 0.25
printf "Launching $SNAME MOOS Community (WARP=%s) \n"  $TIME_WARP
pAntler targ_shoreside.moos >& /dev/null &
printf "Done \n"

uMAC targ_shoreside.moos

printf "Killing all processes ... \n"
ktm
printf "Done killing processes.   \n"



