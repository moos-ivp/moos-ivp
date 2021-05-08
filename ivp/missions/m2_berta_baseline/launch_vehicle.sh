#!/bin/bash 
#---------------------------------------------------------------
#   Script: launch_vehicle.sh                       
#  Mission: m2_berta
#---------------------------------------------------------------
#  Part 1: Set global var defaults
#---------------------------------------------------------------
ME=`basename "$0"`
TIME_WARP=1
JUST_MAKE="no"
VERBOSE="no"
CONFIRM="yes"
AUTO_LAUNCHED="no"
CMD_ARGS=""

IP_ADDR="localhost"
MOOS_PORT="9001"
PSHARE_PORT="9201"

SHORE_IP="localhost"
SHORE_PSHARE="9200"
VNAME="who"
VTYPE="KAYAK"
INDEX="1"

MAXSPD="5"
START_POS="170,-80"  
START_HDG="270"  
START_SPD="0"  
LOITER_POS="x=0,y=-95"

#---------------------------------------------------------------
#  Part 2: Check for and handle command-line arguments
#---------------------------------------------------------------
for ARGI; do
    CMD_ARGS+=" ${ARGI}"
    if [ "${ARGI}" = "--help" -o "${ARGI}" = "-h" ]; then
	echo "$ME [SWITCHES]                                   "
	echo "  --help, -h                                     " 
	echo "    Display this help message                    "
	echo "  --just_make, -j                                " 
	echo "    Just make targ files, but do not launch      "
	echo "  --verbose, -v                                  " 
	echo "    Verbose output, confirm before launching     "
	echo "  --noconfirm, -nc                               "
	echo "    No confirmation before launching             "
	echo "  --auto, -a                                     "
	echo "     Auto-launched by a script.                  "
	echo "     Will not launch uMAC as the final step      "
	echo "                                                 "
	echo "  --ip=<localhost>                               " 
	echo "    Force pHostInfo to use this IP Address       "
	echo "  --mport=<9001>                                 "
	echo "    Port number of this vehicle's MOOSDB port    "
	echo "  --pshare=<9201>                                " 
	echo "    Port number of this vehicle's pShare port    "
	echo "                                                 "
	echo "  --shore=<localhost>                            " 
	echo "    IP address location of shoreside             "
	echo "  --vname=<who>                                  " 
	echo "    Name of the vehicle being launched           " 
	echo "  --vtype=<kayak>                                " 
	echo "    Type name of the vehicle being launched      " 
	echo "  --vgroup=<alpha>                               " 
	echo "    Group name of the vehicle being launched     " 
	echo "  --index=<1>                                    " 
	echo "    Index for setting MOOSDB and pShare ports    "
	echo "                                                 "
	echo "  --maxspd=<m/s>    (default is 5)               "
        echo "    Max speed capability of the vehicle, passed  "
        echo "    to uSimMarine for setting its thrust map     "
	echo "  --start=<X,Y>     (default is 0,0)             " 
	echo "    Start position chosen by script launching    "
	echo "    this script (to ensure separation)           "
	echo "  --hdg=<hdg>       (default is 0)               " 
	echo "    Initial heading of vehicle being launched    " 
	echo "  --spd=<spd>       (default is 0)               " 
	echo "    Initial speed of vehicle being launched      " 
	echo "  --loiter_pos=<0,0>                             " 
	echo "    Center of Initial loiter position            " 
	echo "                                                 "
	exit 0
    elif [ "${ARGI//[^0-9]/}" = "$ARGI" -a "$TIME_WARP" = 1 ]; then 
        TIME_WARP=$ARGI
    elif [ "${ARGI}" = "--just_make" -o "${ARGI}" = "-j" ]; then
	JUST_MAKE="yes"
    elif [ "${ARGI}" = "--verbose" -o "${ARGI}" = "-v" ]; then
	VERBOSE="yes"
    elif [ "${ARGI}" = "--noconfirm" -o "${ARGI}" = "-nc" ]; then
	CONFIRM="no"
    elif [ "${ARGI}" = "--auto" -o "${ARGI}" = "-a" ]; then
	AUTO_LAUNCHED="yes"
	CONFIRM="no"

    elif [ "${ARGI:0:5}" = "--ip=" ]; then
        IP_ADDR="${ARGI#--ip=*}"
    elif [ "${ARGI:0:7}" = "--mport" ]; then
	MOOS_PORT="${ARGI#--mport=*}"
    elif [ "${ARGI:0:9}" = "--pshare=" ]; then
        PSHARE_PORT="${ARGI#--pshare=*}"

    elif [ "${ARGI:0:8}" = "--shore=" ]; then
        SHORE_IP="${ARGI#--shore=*}"
    elif [ "${ARGI:0:8}" = "--vname=" ]; then
	VNAME="${ARGI#--vname=*}"
    elif [ "${ARGI:0:8}" = "--vtype=" ]; then
	VTYPE="${ARGI#--vtype=*}"
    elif [ "${ARGI:0:9}" = "--vgroup=" ]; then
	VGROUP="${ARGI#--vgroup=*}"
    elif [ "${ARGI:0:8}" = "--index=" ]; then
        INDEX="${ARGI#--index=*}"

    elif [ "${ARGI:0:9}" = "--maxspd=" ]; then
        MAXSPD="${ARGI#--maxspd=*}"
    elif [ "${ARGI:0:8}" = "--start=" ]; then
        START_POS="${ARGI#--start=*}"
    elif [ "${ARGI:0:6}" = "--hdg=" ]; then
	START_HDG="${ARGI#--hdg=*}"
    elif [ "${ARGI:0:6}" = "--spd=" ]; then
	START_SPD="${ARGI#--spd=*}"

    elif [ "${ARGI:0:13}" = "--loiter_pos=" ]; then
	LOITER_POS="${ARGI#--loiter_pos=*}"

    else
	echo "$ME: Bad Arg:" ${ARGI} "Exit Code 1"
	echo "Cmd Args:" ${CMD_ARGS} 
	exit 1
    fi
done

MOOS_PORT=`expr $INDEX + 9000`
PSHARE_PORT=`expr $INDEX + 9200`

#---------------------------------------------------------------
#  Part 3: If verbose, show vars and confirm before launching
#---------------------------------------------------------------
if [ "${VERBOSE}" = "yes" -o "${CONFIRM}" = "yes" ]; then 
    echo "$ME"
    echo "CMD_ARGS =      [${CMD_ARGS}]"
    echo "TIME_WARP =     [${TIME_WARP}]"
    echo "AUTO_LAUNCHED = [${AUTO_LAUNCHED}]"
    echo "MOOS_PORT =     [${MOOS_PORT}]"
    echo "PSHARE_PORT =   [${PSHARE_PORT}]"
    echo "IP_ADDR =       [${IP_ADDR}]"
    echo "----------------------------------"
    echo "SHORE_IP =      [${SHORE_IP}]"
    echo "SHORE_PSHARE =  [${SHORE_PSHARE}]"
    echo "VNAME =         [${VNAME}]"
    echo "VTYPE =         [${VTYPE}]"
    echo "VGROUP =        [${VGROUP}]"
    echo "INDEX =         [${INDEX}]"
    echo "----------------------------------"
    echo "MAXSPD =        [${MAXSPD}]"
    echo "START_POS =     [${START_POS}]"
    echo "START_HDG =     [${START_HDG}]"
    echo "START_SPD =     [${START_SPD}]"
    echo "----------------------------------"
    echo "LOITER_POS =    [${LOITER_POS}]"
    echo -n "Hit any key to continue with launching"
    read ANSWER
fi

#---------------------------------------------------------------
#  Part4: Create the .moos and .bhv files. 
#---------------------------------------------------------------
nsplug meta_vehicle.moos targ_$VNAME.moos -f WARP=$TIME_WARP   \
       VNAME=$VNAME                 START_POS=$START_POS       \
       VTYPE=$VTYPE                 START_HDG=$START_HDG       \
       VGROUP=$VGROUP               START_SPD=$START_SPD       \
       IP_ADDR=$IP_ADDR             SHORE_IP=$SHORE_IP         \
       PSHARE_PORT=$PSHARE_PORT     SHORE_PSHARE=$SHORE_PSHARE \
       MOOS_PORT=$MOOS_PORT         MAXSPD=$MAXSPD            

nsplug meta_vehicle.bhv targ_$VNAME.bhv -f VNAME=$VNAME       \
       START_POS=$START_POS         LOITER_POS=$LOITER_POS


if [ ${JUST_MAKE} = "yes" ]; then
    echo "Files assembled; nothing launched; exiting per request."
    exit 0
fi

#---------------------------------------------------------------
#  Part 5: Launch the processes
#---------------------------------------------------------------
echo "Launching $VNAME MOOS Community. WARP is:" $TIME_WARP
pAntler targ_$VNAME.moos >& /dev/null &

#---------------------------------------------------------------
#  Part 6: If launched from script, we're done, exit now
#---------------------------------------------------------------
if [ "${AUTO_LAUNCHED}" = "yes" ]; then
    exit 0
fi

#---------------------------------------------------------------
# Part 7: Launch uMAC until the mission is quit
#---------------------------------------------------------------
uMAC targ_$VNAME.moos
kill -- -$$
