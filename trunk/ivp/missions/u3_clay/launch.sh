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

VNAME1="henry"  # The first vehicle Community
VNAME2="gilda"  # The second vehicle Community
VNAME3="ike"    # The third vehicle Community
VNAME4="james"  # The fourth vehicle Community

START_POS1="0,0"            # Vehicle 1 Behavior configurations
START_POS2="80,0"           # Vehicle 2 Behavior configurations
START_POS3="25,-25"         # Vehicle 3 Behavior configurations
START_POS4="105,-25"        # Vehicle 4 Behavior configurations
LOITER_POS1="x=0,y=-75"
LOITER_POS2="x=125,y=-50"
LOITER_POS3="x=60,y=-100"
LOITER_POS4="x=145,y=-150"

SHORE_LISTEN="9300"

nsplug meta_vehicle.moos targ_gilda.moos -f WARP=$TIME_WARP \
   VNAME=$VNAME1    START_POS=$START_POS1                   \
   VPORT="9001"     LOITER_POS=$LOITER_POS1                 \
   VTYPE="kayak"    SHARE_LISTEN="9301"                     \
   GROUP="GROUP12"  SHORE_LISTEN=$SHORE_LISTEN

nsplug meta_vehicle.moos targ_henry.moos -f WARP=$TIME_WARP \
   VNAME=$VNAME2    START_POS=$START_POS2                   \
   VPORT="9002"     LOITER_POS=$LOITER_POS2                 \
   VTYPE="kayak"    SHARE_LISTEN="9302"                     \
   GROUP="GROUP12"  SHORE_LISTEN=$SHORE_LISTEN

nsplug meta_vehicle.moos targ_ike.moos -f WARP=$TIME_WARP   \
   VNAME=$VNAME3    START_POS=$START_POS3                   \
   VPORT="9003"     LOITER_POS=$LOITER_POS3                 \
   VTYPE="kayak"    SHARE_LISTEN="9303"                     \
   GROUP="GROUP34"  SHORE_LISTEN=$SHORE_LISTEN

nsplug meta_vehicle.moos targ_james.moos -f WARP=$TIME_WARP \
   VNAME=$VNAME4    START_POS=$START_POS4                   \
   VPORT="9004"     LOITER_POS=$LOITER_POS4                 \
   VTYPE="kayak"    SHARE_LISTEN="9304"                     \
   GROUP="GROUP34"  SHORE_LISTEN=$SHORE_LISTEN

nsplug meta_shoreside.moos targ_shoreside.moos -f WARP=$TIME_WARP \
   VNAME="shoreside"  SHARE_LISTEN=$SHORE_LISTEN                  \
   VPORT="9000" 

nsplug meta_vehicle.bhv targ_henry.bhv -f VNAME=$VNAME1  \
    START_POS=$START_POS1 LOITER_POS=$LOITER_POS1       

nsplug meta_vehicle.bhv targ_gilda.bhv -f VNAME=$VNAME2  \
    START_POS=$START_POS1 LOITER_POS=$LOITER_POS2       

nsplug meta_vehicle.bhv targ_ike.bhv -f   VNAME=$VNAME3  \
    START_POS=$START_POS3 LOITER_POS=$LOITER_POS3       

nsplug meta_vehicle.bhv targ_james.bhv -f VNAME=$VNAME4  \
    START_POS=$START_POS4 LOITER_POS=$LOITER_POS4       

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

printf "Launching $VNAME3 MOOS Community (WARP=%s) \n" $TIME_WARP
pAntler targ_ike.moos >& /dev/null &
sleep 0.25
printf "Launching $VNAME4 MOOS Community (WARP=%s) \n" $TIME_WARP
pAntler targ_james.moos >& /dev/null &
sleep 0.25

printf "Launching $SNAME MOOS Community (WARP=%s) \n"  $TIME_WARP
pAntler targ_shoreside.moos >& /dev/null &
printf "Done \n"

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
    kill %1 %2 %3 %4 %5
    printf "Done killing processes.   \n"
fi


