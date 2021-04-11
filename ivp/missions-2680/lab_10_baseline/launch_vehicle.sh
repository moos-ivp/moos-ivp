#!/bin/bash 
#-------------------------------------------------------
#   Script: launch_vehicle.sh                       
#  Mission: lab_10_baseline
#-------------------------------------------------------
#  Part 1: Set global var defaults
#----------------------------------------------------------
TIME_WARP=1
JUST_MAKE="no"
HOSTNAME=$(hostname -s)
VNAME=$(id -un)

IP_ADDR="localhost"
MOOS_PORT="9001"
PSHARE_PORT="9201"
SHORE_PSHARE="9200"
SHORE_IP="localhost"
GUI="yes"
CONF="yes"
UMAC="no"

#-------------------------------------------------------
#  Part 2: Check for and handle command-line arguments
#-------------------------------------------------------
for ARGI; do
    if [ "${ARGI}" = "--help" -o "${ARGI}" = "-h" ]; then
	echo "launch_vehicle.sh [SWITCHES]                     "
	echo "  --help, -h                                     " 
	echo "    Display this help message                    "
	echo "  --just_make, -j                                " 
	echo "    Just make targ files, but do not launch      "
	echo "                                                 "
	echo "  --vname=<vname>                                " 
	echo "    Name of the vehicle being launched           " 
	echo "                                                 "
	echo "  --shore=<ipaddr>                               "
	echo "  --shoreip=<ipaddr>                             "
	echo "    IP address of the shoreside                  "
	echo "                                                 "
	echo "  --mport=<port>(9001)                           "
	echo "    Port number of this vehicle's MOOSDB port    "
	echo "                                                 "
	echo "  --pshare=<port>                                " 
	echo "    Port number of this vehicle's pShare port    "
	echo "                                                 "
	echo "  --ip=<ipaddr>                                  " 
	echo "    Force pHostInfo to use this IP Address       "
	echo "                                                 "
	echo "  --nogui                                        " 
	echo "    Do not launch pMarineViewer GUI with vehicle "
	echo "  --umac,-u                                      " 
	echo "    Launch uMAC after launching the vehicle      "
	echo "  --nc,-nc                                       " 
	echo "    No confirmation before launching             "
	exit 0
    elif [ "${ARGI//[^0-9]/}" = "$ARGI" -a "$TIME_WARP" = 1 ]; then 
        TIME_WARP=$ARGI
    elif [ "${ARGI}" = "--just_make" -o "${ARGI}" = "-j" ]; then
	JUST_MAKE="yes"
    elif [ "${ARGI}" = "-nc" -o "${ARGI}" = "--nc" ]; then
	CONF="no"
    elif [ "${ARGI}" = "-u" -o "${ARGI}" = "--umac" ]; then
	UMAC="yes"
    elif [ "${ARGI:0:8}" = "--shore=" ]; then
	SHORE_IP="${ARGI#--shore=*}"
    elif [ "${ARGI:0:10}" = "--shoreip=" ]; then
	SHORE_IP="${ARGI#--shoreip=*}"
    elif [ "${ARGI:0:5}" = "--ip=" ]; then
        IP_ADDR="${ARGI#--ip=*}"
    elif [ "${ARGI:0:8}" = "--mport=" ]; then
	MOOS_PORT="${ARGI#--mport=*}"
    elif [ "${ARGI:0:9}" = "--pshare=" ]; then
	PSHARE_PORT="${ARGI#--pshare=*}"
    elif [ "${ARGI:0:8}" = "--vname=" ]; then
	VNAME="${ARGI#--vname=*}"
    elif [ "${ARGI}" = "--nogui" ]; then
	GUI="no"
    else
	echo "launch_vehicle.sh: Bad Arg: $ARGI Exit Code 1"
	exit 1
    fi
done

if [ "${CONF}" = "yes" ]; then 
    echo "PSHARE_PORT =  [${PSHARE_PORT}]"
    echo "MOOS_PORT =    [${MOOS_PORT}]"
    echo "IP_ADDR =      [${IP_ADDR}]"
    echo "VNAME =        [${VNAME}]"
    echo "SHORE_IP =     [${SHORE_IP}]"
    echo "SHORE_PSHARE = [${SHORE_PSHARE}]"
    echo "TIME_WARP =    [${TIME_WARP}]"
    echo "GUI =          [${GUI}]"
    echo "UMAC =         [${UMAC}]"
    echo -n "Hit any key to continue with launching"
    read ANSWER
fi

#-------------------------------------------------------
#  Part 3: Create the .moos and .bhv files. 
#-------------------------------------------------------

FULL_VNAME=$VNAME"@"$HOSTNAME
WPT_COLOR="light_blue"

# Generate a random start position in range x=[0,180], y=[0,-50]
X_START_POS=$(($RANDOM % 180))
Y_START_POS=$((($RANDOM % 50)  - 50))
# Generate a random start position in range x=[0,180], y=[0,-125]
X_LOITER_POS=$(($RANDOM % 180))
Y_LOITER_POS=$((($RANDOM % 50) - 125))
START_POS="$X_START_POS,$Y_START_POS" 
LOITER_POS="x=$X_LOITER_POS,y=$Y_LOITER_POS" 

nsplug meta_vehicle.moos targ_$FULL_VNAME.moos -f WARP=$TIME_WARP  \
    PSHARE_PORT=$PSHARE_PORT     VNAME=$FULL_VNAME                 \
    START_POS=$START_POS         SHORE_IP=$SHORE_IP                \
    SHORE_PSHARE=$SHORE_PSHARE   VPORT=$MOOS_PORT                  \
    IP_ADDR=$IP_ADDR             GUI=$GUI

nsplug meta_vehicle.bhv targ_$FULL_VNAME.bhv -f VNAME=$FULL_VNAME  \
    START_POS=$START_POS LOITER_POS=$LOITER_POS       
   
 
if [ ${JUST_MAKE} = "yes" ]; then
    exit 0
fi

#-------------------------------------------------------
#  Part 4: Launch the processes
#-------------------------------------------------------
printf "Launching $VNAME MOOS Community (WARP=%s) \n" $TIME_WARP
pAntler targ_$FULL_VNAME.moos >& /dev/null &

#-------------------------------------------------------
#  Part 5: Launch the GUI if desired
#-------------------------------------------------------
if [ "${GUI}" = "no" -a "${UMAC}" = "no" ]; then
    exit 0
fi

uMAC targ_$FULL_VNAME.moos

#-------------------------------------------------------
#  Part 6: Exiting and/or killing the simulation
#-------------------------------------------------------
kill -- -$$
