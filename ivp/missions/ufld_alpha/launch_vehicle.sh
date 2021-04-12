#!/bin/bash -e
#-------------------------------------------------------------- 
#   Script: launch_vehicle.sh                                    
#   Author: Michael Benjamin  
#     Date: April 2020     
#--------------------------------------------------------------
#  Part 1: Declare global var defaults
#--------------------------------------------------------------
TIME_WARP=1
JUST_MAKE="no"
AUTO=""
VNAME="abe"

IP_ADDR="localhost"
INDEX="1"
SHORE_PSHARE="9201"
SHORE_IP="localhost"
PSHARE_PORT="9201"

REGION="forest_lake"
START_POS="0,0"  


#--------------------------------------------------------------
#  Part 2: Check for and handle command-line arguments
#--------------------------------------------------------------
for ARGI; do
    if [ "${ARGI}" = "--help" -o "${ARGI}" = "-h" ]; then
	echo "launch_vehicle.sh [SWITCHES] [time_warp]         "
	echo "  --just_make, -j                                " 
	echo "  --vname=VNAME                                  " 
	echo "  --index=INDEX                                  " 
	echo "  --help, -h                                     " 
	echo "  --ip=<addr>       (default is localhost)       " 
	echo "  --startpos=X,Y    (Default is 0,0)             " 
	echo "  --shore=IP:PORT   (Default is localhost:9300)  " 
	echo "  --pshare=PORT     (Default is 9301)            " 
	echo "  --pavlab, -p      Set region to be MIT pavlab  " 
        echo "  --auto, -a        Auto-launched. uMAC not used."
	exit 0;
    elif [ "${ARGI//[^0-9]/}" = "$ARGI" -a "$TIME_WARP" = 1 ]; then 
        TIME_WARP=$ARGI
    elif [ "${ARGI}" = "--just_make" -o "${ARGI}" = "-j" ]; then
	JUST_MAKE="yes"
    elif [ "${ARGI}" = "--auto" -o "${ARGI}" = "-a" ]; then
        AUTO="yes"
    elif [ "${ARGI}" = "--pavlab" -o "${ARGI}" = "-p" ]; then
        REGION="pavlab"
    elif [ "${ARGI:0:8}" = "--vname=" ]; then
        VNAME="${ARGI#--vname=*}"
    elif [ "${ARGI:0:8}" = "--index=" ]; then
        INDEX="${ARGI#--index=*}"
    elif [ "${ARGI:0:5}" = "--ip=" ]; then
        IP_ADDR="${ARGI#--ip=*}"
    elif [ "${ARGI:0:11}" = "--startpos=" ]; then
        START_POS="${ARGI#--startpos=*}"
    elif [ "${ARGI:0:8}" = "--shore=" ]; then
        SHORE="${ARGI#--shore=*}"
    elif [ "${ARGI:0:9}" = "--pshare=" ]; then
        PSHARE_PORT="${ARGI#--pshare=*}"
    else 
	echo "launch_vehicle.sh: Bad Arg: " $ARGI " Exit Code 1."
	exit 1
    fi
done

if [ "${PSHARE_PORT}" = "" ] ; then
    PSHARE_PORT="930"$INDEX
fi

if [ "${INDEX}" != "1" ] ; then
    VNAME=$VNAME"_"$INDEX
fi


#--------------------------------------------------------------
#  Part 3: Create the .moos and .bhv files. 
#--------------------------------------------------------------
NSFLAGS="-s -f"
if [ "${AUTO}" = "" ]; then
    NSFLAGS="-i -f"
fi

nsplug meta_vehicle.moos targ_$VNAME.moos $NSFLAGS WARP=$TIME_WARP \
       VNAME=$VNAME            START_POS=$START_POS                \
       VPORT="900"$INDEX       SHORE_PSHARE=$SHORE_PSHARE          \
       IP_ADDR=$IP_ADDR        SHORE_IP=$SHORE_IP                  \
       REGION=$REGION          PSHARE_PORT=$PSHARE_PORT

nsplug meta_vehicle.bhv targ_$VNAME.bhv $NSFLAGS VNAME=$VNAME      \
       START_POS=$START_POS    REGION=$REGION

if [ ${JUST_MAKE} = "yes" ] ; then
    echo "Files assembled; nothing launched; exiting per request."
    exit 0
fi

#--------------------------------------------------------------
#  Part 4: Launch the processes
#--------------------------------------------------------------
echo "Launching $VNAME MOOS Community, WARP:" $TIME_WARP
pAntler targ_$VNAME.moos >& /dev/null &
echo "Done Launching the vehicle mission."

#-------------------------------------------------------------- 
#  Part 5: Unless auto-launched, launch uMAC until mission quit          
#-------------------------------------------------------------- 
if [ "${AUTO}" = "" ]; then
    uMAC targ_$VNAME.moos
    kill -- -$$
fi


