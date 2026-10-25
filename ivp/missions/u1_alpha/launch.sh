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
        echo "launch.sh Bad arg:" $ARGI "Exiting with code: 1"
        exit 1
    fi
done

#-------------------------------------------------------
#  Part 2: Create the .moos and .bhv files. 
#-------------------------------------------------------
VNAME="henry"   
START_POS="0,0"         
LOITER_POS="x=0,y=-75"
SHORE_LISTEN="9300"

nsplug meta_vehicle.moos targ_henry.moos -f WARP=$TIME_WARP \
    VNAME=$VNAME           SHARE_LISTEN="9301"              \
    VPORT="9001"           SHORE_LISTEN=$SHORE_LISTEN       \
    START_POS=$START_POS 

nsplug meta_shoreside.moos targ_shoreside.moos -f WARP=$TIME_WARP \
    SNAME="shoreside"  SHARE_LISTEN=$SHORE_LISTEN  SPORT="9000"       

nsplug meta_vehicle.bhv targ_henry.bhv -f VNAME=$VNAME     \
    START_POS=$START_POS LOITER_POS=$LOITER_POS       

if [ ! -e targ_henry.moos ]; then echo "no targ_henry.moos"; exit; fi
if [ ! -e targ_henry.bhv  ]; then echo "no targ_henry.bhv "; exit; fi
if [ ! -e targ_shoreside.moos ]; then echo "no targ_shoreside.moos";  exit; fi

if [ ${JUST_MAKE} = "yes" ] ; then
    exit 0
fi


#-------------------------------------------------------
#  Part 3: Launch the processes
#-------------------------------------------------------
printf "Launching $SNAME MOOS Community (WARP=%s) \n"  $TIME_WARP
pAntler targ_shoreside.moos >& /dev/null &
printf "Launching $VNAME1 MOOS Community (WARP=%s) \n" $TIME_WARP
pAntler targ_henry.moos >& /dev/null &
printf "Done \n"

uMAC targ_shoreside.moos

printf "Killing all processes ... \n"
mykill
printf "Done killing processes.   \n"
