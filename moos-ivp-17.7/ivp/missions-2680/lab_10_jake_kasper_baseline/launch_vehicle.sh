#!/bin/bash 
#-------------------------------------------------------
#  Part 1: Check for and handle command-line arguments
#-------------------------------------------------------
TIME_WARP=1
JUST_BUILD="no"
VNAME="anonymous"
MOOS_PORT="9001"
UDP_LISTEN_PORT="9201"
SHOREIP="localhost"
SHORE_LISTEN="9200"

for ARGI; do
    if [ "${ARGI}" = "--help" -o "${ARGI}" = "-h" ] ; then
	printf "%s [SWITCHES] [time_warp]   \n" $0
	printf "  --shore=IP address of shoreside       \n" 
	printf "  --mport=MOOSDB Port #                 \n" 
	printf "  --lport=pShare UDPListen Port #       \n" 
	printf "  --just_make, -j    \n" 
	printf "  --vname=VNAME      \n" 
	printf "  --help, -h         \n" 
	exit 0;
    elif [ "${ARGI:0:8}" = "--vname=" ] ; then
        VNAME="${ARGI#--vname=*}"
    elif [ "${ARGI//[^0-9]/}" = "$ARGI" -a "$TIME_WARP" = 1 ]; then 
        TIME_WARP=$ARGI
    elif [ "${ARGI:0:8}" = "--shore=" ] ; then
	SHOREIP="${ARGI#--shore=*}"
    elif [ "${ARGI:0:7}" = "--mport" ] ; then
	MOOS_PORT="${ARGI#--mport=*}"
    elif [ "${ARGI:0:7}" = "--lport" ] ; then
	UDP_LISTEN_PORT="${ARGI#--lport=*}"
    elif [ "${ARGI}" = "--just_build" -o "${ARGI}" = "-j" ] ; then
	JUST_BUILD="yes"
    else 
	printf "Bad Argument: %s \n" $ARGI
	exit 0
    fi
done


#-------------------------------------------------------
#  Part 2: Create the .moos and .bhv files. 
#-------------------------------------------------------
VNAME1="jake"      # The first   vehicle community
VNAME2="kasper"    # The second  vehicle community
START_POS="0,0"  

nsplug meta_vehicle.moos targ_$VNAME.moos -f WARP=$TIME_WARP \
    VNAME=$VNAME                       VPORT=$MOOS_PORT      \
    SHARE_LISTEN=$UDP_LISTEN_PORT      START_POS=$START_POS  \
    SHOREIP=$SHOREIP                   VTYPE=UUV             \
    VNAME1=$VNAME1                     VNAME2=$VNAME2        \
    SHORE_LISTEN=$SHORE_LISTEN  

nsplug meta_vehicle.bhv targ_$VNAME.bhv -f VNAME=$VNAME  \
    START_POS=$START_POS VNAME1=$VNAME1 VNAME2=$VNAME2

if [ ${JUST_BUILD} = "yes" ] ; then
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


