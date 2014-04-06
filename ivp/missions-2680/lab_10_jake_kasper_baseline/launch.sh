#!/bin/bash 
#-------------------------------------------------------
#  Part 1: Check for and handle command-line arguments
#-------------------------------------------------------
TIME_WARP=1
JUST_MAKE="no"
HAZARD_FILE="hazards.txt"
PEN_MISSED_HAZ=150
PEN_FALARM=25
MAX_TIME=7200

for ARGI; do
    if [ "${ARGI}" = "--help" -o "${ARGI}" = "-h" ] ; then
	printf "%s [SWITCHES] [time_warp]   \n" $0
	printf "  --just_make, -j    \n" 
	printf "  --hazards=file.txt \n" 
	printf "  --help, -h         \n" 
	exit 0;
    elif [ "${ARGI:0:10}" = "--hazards=" ] ; then
        HAZARD_FILE="${ARGI#--hazards=*}"

    elif [ "${ARGI:0:11}" = "--max_time=" ] ; then
        MAX_TIME="${ARGI#--max_time=*}"
    elif [ "${ARGI:0:17}" = "--pen_missed_haz=" ] ; then
        PEN_MISSED_HAZ="${ARGI#--pen_missed_haz=*}"
    elif [ "${ARGI:0:13}" = "--pen_falarm=" ] ; then
        PEN_FALARM="${ARGI#--pen_falarm=*}"

    elif [ "${ARGI//[^0-9]/}" = "$ARGI" -a "$TIME_WARP" = 1 ]; then 
        TIME_WARP=$ARGI
    elif [ "${ARGI}" = "--just_build" -o "${ARGI}" = "-j" ] ; then
	JUST_MAKE="yes"
    else 
	printf "Bad Argument: %s \n" $ARGI
	exit 0
    fi
done

if [ -f $HAZARD_FILE ]; then
  echo "          Hazard File: $HAZARD_FILE"
  echo "             Max Time: $MAX_TIME"
  echo "Penalty Missed Hazard: $PEN_MISSED_HAZ"
  echo "  Penalty False Alarm: $PEN_FALARM"
else
  echo "$HAZARD_FILE not found. Exiting"
  exit 0
fi 

#-------------------------------------------------------
#  Part 2: Create the .moos and .bhv files. 
#-------------------------------------------------------
VNAME1="jake"      # The first   vehicle community
VNAME2="kasper"    # The second  vehicle community
#START_POS1="450,-20"  
#START_POS2="440,0"  
START_POS1="-30,-20"  
START_POS2="45,0"  

# What is nsplug? Type "nsplug --help" or "nsplug --manual"

nsplug meta_shoreside.moos targ_shoreside.moos -f WARP=$TIME_WARP \
    VNAME="shoreside"         HAZARD_FILE=$HAZARD_FILE            \
    SHOREIP=localhost         SHORE_LISTEN=9200                   \
    MAX_TIME=$MAX_TIME        PEN_MISSED_HAZ=$PEN_MISSED_HAZ      \
    PEN_FALARM=$PEN_FALARM

nsplug meta_vehicle.moos targ_$VNAME1.moos -f WARP=$TIME_WARP  \
    VNAME=$VNAME1         START_POS=$START_POS1                \
    VPORT="9001"          SHARE_LISTEN="9301"                  \
    SHOREIP="localhost"   SHORE_LISTEN="9200"                  \
    VNAME1=$VNAME1        VNAME2=$VNAME2                       \
    VTYPE=UUV 

nsplug meta_vehicle.moos targ_$VNAME2.moos -f WARP=$TIME_WARP  \
    VNAME=$VNAME2         START_POS=$START_POS2                \
    VPORT="9002"          SHARE_LISTEN="9302"                  \
    SHOREIP="localhost"   SHORE_LISTEN="9200"                  \
    VNAME1=$VNAME1        VNAME2=$VNAME2                       \
    VTYPE=UUV 

nsplug meta_vehicle.bhv targ_$VNAME1.bhv -f VNAME=$VNAME1   \
    START_POS=$START_POS1 VNAME1=$VNAME1 VNAME2=$VNAME2

nsplug meta_vehicle.bhv targ_$VNAME2.bhv -f VNAME=$VNAME2   \
    START_POS=$START_POS2  VNAME1=$VNAME1 VNAME2=$VNAME2

if [ ${JUST_MAKE} = "yes" ] ; then
    exit 0
fi

#-------------------------------------------------------
#  Part 3: Launch the processes
#-------------------------------------------------------
printf "Launching $VNAME1 MOOS Community (WARP=%s) \n" $TIME_WARP
pAntler targ_$VNAME1.moos >& /dev/null &
printf "Launching $VNAME2 MOOS Community (WARP=%s) \n" $TIME_WARP
pAntler targ_$VNAME2.moos >& /dev/null &
printf "Launching $SNAME MOOS Community (WARP=%s) \n"  $TIME_WARP
pAntler targ_shoreside.moos >& /dev/null &
printf "Done \n"

uMAC targ_shoreside.moos

printf "Killing all processes ... \n"
kill %1 %2 %3 
mykill
printf "Done killing processes.   \n"


