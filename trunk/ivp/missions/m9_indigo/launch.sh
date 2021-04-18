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
    elif [ "${ARGI}" = "--just_make" -o "${ARGI}" = "-j" ] ; then
	JUST_MAKE="yes"
    else 
        echo "launch.sh Bad arg:" $ARGI " Exiting with code: 1"
        exit 1
    fi
done

#-------------------------------------------------------
#  Part 2: Create the .moos and .bhv files. 
#-------------------------------------------------------
VNAME1="archie"        # The first  surface vehicle community
VNAME2="betty"         # The second surface vehicle community

START_POS1="0,0"       
START_POS2="10,-10"    
START_POS9="-180,-100" 
SHORE_LISTEN="9300"

nsplug meta_surfacecraft.moos targ_archie.moos -f WARP=$TIME_WARP \
   VNAME=$VNAME1    START_POS=$START_POS1                         \
   VPORT="9001"     SHARE_LISTEN="9301"                           \
   VTYPE=KAYAK      SHORE_LISTEN=$SHORE_LISTEN

nsplug meta_surfacecraft.moos targ_betty.moos -f WARP=$TIME_WARP  \
   VNAME=$VNAME2    START_POS=$START_POS2                         \
   VPORT="9002"     SHARE_LISTEN="9302"                           \
   VTYPE=KAYAK      SHORE_LISTEN=$SHORE_LISTEN

nsplug meta_shoreside.moos targ_shoreside.moos -f WARP=$TIME_WARP \
   SNAME="shoreside"  SHARE_LISTEN=$SHORE_LISTEN                  \
   SPORT="9000"       

nsplug meta_archie.bhv targ_archie.bhv -f VNAME=$VNAME1   \
    START_POS=$START_POS1 

nsplug meta_betty.bhv targ_betty.bhv -f VNAME=$VNAME2     \
    START_POS=$START_POS2 

if [ ${JUST_MAKE} = "yes" ] ; then
    exit 0
fi

#-------------------------------------------------------
#  Part 3: Launch the processes
#-------------------------------------------------------
printf "Launching $VNAME1 MOOS Community (WARP=%s) \n" $TIME_WARP
pAntler targ_archie.moos >& /dev/null &
sleep 0.25
printf "Launching $VNAME2 MOOS Community (WARP=%s) \n" $TIME_WARP
pAntler targ_betty.moos >& /dev/null &
sleep 0.25
printf "Launching $SNAME MOOS Community (WARP=%s) \n"  $TIME_WARP
pAntler targ_shoreside.moos >& /dev/null &
printf "Done \n"

#-------------------------------------------------------
#  Part 4: Exiting and/or killing the simulation
#-------------------------------------------------------
ANSWER="0"
while [ "${ANSWER}" != "2" -a "${ANSWER}" != "q" ]; do
    printf "Hit (q) to Exit and Kill Simulation \n"
    printf "> "
    read ANSWER
done

# %1, %2, %3 matches the PID of the first three jobs in the active
# jobs list, namely the three pAntler jobs launched in Part 3.
if [ "${ANSWER}" = "q"  -o "${ANSWER}" = "2" ]; then
    printf "Killing all processes ... \n"
    kill %1 %2 %3 %4
    printf "Done killing processes.   \n"
fi

