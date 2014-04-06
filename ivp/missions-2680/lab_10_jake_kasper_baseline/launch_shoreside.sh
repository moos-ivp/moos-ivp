#!/bin/bash 
#-------------------------------------------------------
#  Part 1: Check for and handle command-line arguments
#-------------------------------------------------------
TIME_WARP=1
JUST_BUILD="no"
HAZARD_FILE="hazards.txt"
PEN_MISSED_HAZ=150
PEN_FALARM=25
MAX_TIME=7200

for ARGI; do
    if [ "${ARGI}" = "--help" -o "${ARGI}" = "-h" ] ; then
	printf "%s [SWITCHES] [time_warp]   \n" $0
	printf "  --just_build, -j          \n" 
	printf "  --hazards=file.txt        \n" 
	printf "  --max_time=N              \n" 
	printf "  --pen_missed_haz=N        \n" 
	printf "  --pen_falarm=N            \n" 
	printf "  --help, -h                \n" 
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
	JUST_BUILD="yes"
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
#  Part 2: Create the .moos file(s)
#-------------------------------------------------------
nsplug meta_shoreside.moos targ_shoreside.moos -f WARP=$TIME_WARP \
    VNAME="shoreside"         HAZARD_FILE=$HAZARD_FILE            \
    SHOREIP=localhost         SHORE_LISTEN=9200                   \
    MAX_TIME=$MAX_TIME        PEN_MISSED_HAZ=$PEN_MISSED_HAZ      \
    PEN_FALARM=$PEN_FALARM

if [ ${JUST_BUILD} = "yes" ] ; then
    exit 0
fi

#-------------------------------------------------------
#  Part 3: Launch the processes
#-------------------------------------------------------
printf "Launching $SNAME MOOS Community (WARP=%s) \n"  $TIME_WARP
pAntler targ_shoreside.moos >& /dev/null &
printf "Done \n"

uMAC targ_shoreside.moos

printf "Killing all processes ... \n"
kill %1
sleep 1
mykill
printf "Done killing processes.   \n"


