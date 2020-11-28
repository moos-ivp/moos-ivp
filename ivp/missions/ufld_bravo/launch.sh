#!/bin/bash 
#-------------------------------------------------------
#  Part 1: Check for and handle command-line arguments
#-------------------------------------------------------
TIME_WARP=1
JUST_MAKE="no"
for ARGI; do
    if [ "${ARGI}" = "--help" -o "${ARGI}" = "-h" ] ; then
	echo "launch.sh [SWITCHES] [time_warp]   "
	echo "  --just_make, -j    " 
	echo "  --help, -h         " 
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
VNAME1="archie"      # The first   vehicle community
VNAME2="betty"       # The second  vehicle community
VNAME3="charlie"     # The third   vehicle community
START_POS1="28,0"  
START_POS2="30,0"  
START_POS3="32,0"  
SPEED1="2.0"
SPEED2="2.5"
SPEED3="3.0"
DELAY_TIME1=0
DELAY_TIME2=30
DELAY_TIME3=60
SHORE="multicast_8"

# What is nsplug? Type "nsplug --help" or "nsplug --manual"

nsplug meta_shoreside.moos targ_shoreside.moos -f WARP=$TIME_WARP \
   VNAME="shoreside" 

#--------------------------------
nsplug meta_vehicle.moos targ_$VNAME1.moos -f WARP=$TIME_WARP  VTYPE=UUV \
   VNAME=$VNAME1      START_POS=$START_POS1  SHORE=$SHORE                \
   VPORT="9001"       SHARE_LISTEN="9301"    

nsplug meta_vehicle.bhv targ_$VNAME1.bhv -f VNAME=$VNAME1     \
    START_POS=$START_POS1   SPEED=$SPEED1 ORDER="normal"      \
    DELAYTIME=$DELAY_TIME1


#--------------------------------
nsplug meta_vehicle.moos targ_$VNAME2.moos -f WARP=$TIME_WARP  VTYPE=UUV \
   VNAME=$VNAME2      START_POS=$START_POS2   SHORE=$SHORE               \
   VPORT="9002"       SHARE_LISTEN="9302"    

nsplug meta_vehicle.bhv targ_$VNAME2.bhv -f VNAME=$VNAME2     \
    START_POS=$START_POS2 SPEED=$SPEED2 ORDER="normal"       \
    DELAYTIME=$DELAY_TIME2


#--------------------------------
nsplug meta_vehicle.moos targ_$VNAME3.moos -f WARP=$TIME_WARP  VTYPE=UUV \
   VNAME=$VNAME3      START_POS=$START_POS3  SHORE=$SHORE                \
   VPORT="9003"       SHARE_LISTEN="9303"    

nsplug meta_vehicle.bhv targ_$VNAME3.bhv -f VNAME=$VNAME3     \
    START_POS=$START_POS3  SPEED=$SPEED3 ORDER="normal"       \
    DELAYTIME=$DELAY_TIME3

if [ ${JUST_MAKE} = "yes" ] ; then
    exit 0
fi

#-------------------------------------------------------
#  Part 3: Launch the processes
#-------------------------------------------------------
printf "Launching $VNAME1 MOOS Community (WARP=%s) \n" $TIME_WARP
pAntler targ_$VNAME1.moos >& /dev/null &
sleep .25
printf "Launching $VNAME2 MOOS Community (WARP=%s) \n" $TIME_WARP
pAntler targ_$VNAME2.moos >& /dev/null &
sleep .25
printf "Launching $VNAME3 MOOS Community (WARP=%s) \n" $TIME_WARP
pAntler targ_$VNAME3.moos >& /dev/null &
sleep .25
printf "Launching $SNAME MOOS Community (WARP=%s) \n"  $TIME_WARP
pAntler targ_shoreside.moos >& /dev/null &
printf "Done \n"

uMAC targ_shoreside.moos

printf "Killing all processes ... \n"
kill %1 %2 %3 %4
printf "Done killing processes.   \n"


