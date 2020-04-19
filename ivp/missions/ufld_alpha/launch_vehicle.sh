#!/bin/bash 
#--------------------------------------------------------------
#   Script: launch_shoreside.sh                                    
#   Author: Michael Benjamin  
#     Date: April 2020     
#--------------------------------------------------------------
#----------------------------------------------------------  
#  Part 1: Set Exit actions and declare global var defaults
#----------------------------------------------------------
trap "kill -- -$$" EXIT SIGTERM SIGHUP SIGINT SIGKILL
TIME_WARP=1
JUST_MAKE="no"
IP_ADDR="localhost"
VNAME=$(id -un)
INDEX="1"
SHORE=""
START_POS="0,0"  
SHORE="localhost:9300"
PSHARE_PORT=""

#-------------------------------------------------------
#  Part 1: Check for and handle command-line arguments
#-------------------------------------------------------
for ARGI; do
    if [ "${ARGI}" = "--help" -o "${ARGI}" = "-h" ] ; then
	echo "launch_vehicle.sh [SWITCHES] [time_warp]        "
	echo "  --just_make, -j                               " 
	echo "  --vname=VNAME                                 " 
	echo "  --index=INDEX                                 " 
	echo "  --help, -h                                    " 
	echo "  --ip=<addr>       (default is localhost)      " 
	echo "  --startpos=X,Y    (Default is 0,0)            " 
	echo "  --shore=IP:PORT   (Default is localhost:9300) " 
	echo "  --pshare=PORT     (Default is 9301)           " 
	exit 0;
    elif [ "${ARGI//[^0-9]/}" = "$ARGI" -a "$TIME_WARP" = 1 ]; then 
        TIME_WARP=$ARGI
    elif [ "${ARGI:0:8}" = "--vname=" ] ; then
        VNAME="${ARGI#--vname=*}"
	echo "VNAME is" $VNAME
    elif [ "${ARGI:0:8}" = "--index=" ] ; then
        INDEX="${ARGI#--index=*}"
    elif [ "${ARGI:0:5}" = "--ip=" ]; then
        IP_ADDR="${ARGI#--ip=*}"
    elif [ "${ARGI:0:11}" = "--startpos=" ] ; then
        START_POS="${ARGI#--startpos=*}"
    elif [ "${ARGI:0:8}" = "--shore=" ] ; then
        SHORE="${ARGI#--shore=*}"
    elif [ "${ARGI}" = "--just_make" -o "${ARGI}" = "-j" ] ; then
	JUST_MAKE="yes"
    elif [ "${ARGI:0:9}" = "--pshare=" ]; then
        PSHARE_PORT="${ARGI#--pshare=*}"
    else 
	echo "launch_vehicle.sh: Bad Arg: " $ARGI
	exit 0
    fi
done

if [ "${PSHARE_PORT}" = "" ] ; then
    PSHARE_PORT="930"$INDEX
fi

VNAME=$VNAME"_"$INDEX

#-------------------------------------------------------
#  Part 3: Create the .moos and .bhv files. 
#-------------------------------------------------------
# What is nsplug? Type "nsplug --help" or "nsplug --manual"

nsplug meta_vehicle.moos targ_$VNAME.moos -f WARP=$TIME_WARP  VTYPE=KAYAK  \
   VNAME=$VNAME        START_POS=$START_POS                            \
   VPORT="900"$INDEX   PSHARE_PORT=$PSHARE_PORT                        \
   IP_ADDR=$IP_ADDR    SHORE=$SHORE  

nsplug meta_vehicle.bhv targ_$VNAME.bhv -f VNAME=$VNAME START_POS=$START_POS

if [ ${JUST_MAKE} = "yes" ] ; then
    exit 0
fi

#-------------------------------------------------------
#  Part 3: Launch the processes
#-------------------------------------------------------
echo "Launching $VNAME MOOS Community, WARP:" $TIME_WARP

pAntler targ_$VNAME.moos >& /dev/null &
sleep .25
echo "Done Launching the vehicle mission."

uMAC targ_$VNAME.moos

# Killing of all launched procs handled by the trap setting 
# configured at the top of this script

