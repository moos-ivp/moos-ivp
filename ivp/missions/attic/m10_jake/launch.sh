#!/bin/bash 
#-------------------------------------------------------
#  Part 1: Check for and handle command-line arguments
#-------------------------------------------------------
TIME_WARP=1
JUST_MAKE="no"
HAZARD_FILE="hazards.txt"
for ARGI; do
    if [ "${ARGI}" = "--help" -o "${ARGI}" = "-h" ] ; then
	printf "%s [SWITCHES] [time_warp]   \n" $0
	printf "  --just_make, -j    \n" 
        printf "  --hazards=file.txt \n"
	printf "  --help, -h         \n" 
	exit 0;
    elif [ "${ARGI:0:10}" = "--hazards=" ] ; then
        HAZARD_FILE="${ARGI#--hazards=*}"
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
  echo "Using Hazard File $HAZARD_FILE"
else
  echo "$HAZARD_FILE not found. Exiting"
  exit 0
fi

#-------------------------------------------------------
#  Part 2: Create the .moos and .bhv files. 
#-------------------------------------------------------
VNAME1="archie"    # The first  vehicle community
START_POS1="0,0"  

nsplug meta_archie.moos targ_archie.moos -f WARP=$TIME_WARP  \
   VNAME=$VNAME1      START_POS=$START_POS1                  \
   VPORT="9001"       SHARE_LISTEN="9301"                    \
   VTYPE=UUV          

nsplug meta_shoreside.moos targ_shoreside.moos -f WARP=$TIME_WARP \
   HAZARD_FILE=$HAZARD_FILE

nsplug meta_archie.bhv targ_archie.bhv -f VNAME=$VNAME1      \
    START_POS=$START_POS1 

if [ ${JUST_MAKE} = "yes" ] ; then
    exit 0
fi

#-------------------------------------------------------
#  Part 3: Launch the processes
#-------------------------------------------------------
printf "Launching $VNAME1 MOOS Community (WARP=%s) \n" $TIME_WARP
pAntler targ_$VNAME1.moos >& /dev/null &
sleep .25
printf "Launching Shoreside MOOS Community (WARP=%s) \n"  $TIME_WARP
pAntler targ_shoreside.moos >& /dev/null &
printf "Done \n"

uMAC targ_shoreside.moos

printf "Killing all processes ... \n"
kill %1 %2 
printf "Done killing processes.   \n"


