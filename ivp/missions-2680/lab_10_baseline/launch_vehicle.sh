#!/bin/bash 
#-------------------------------------------------------
#  Part 1: Check for and handle command-line arguments
#-------------------------------------------------------
TIME_WARP=1
JUST_MAKE="no"
HOSTNAME=$(hostname -s)
VNAME=$(id -un)

IP_ADDR=""
MOOS_PORT="9001"
PSHARE_PORT="9301"
SHORE="localhost:9300"
GUI="yes"

#-------------------------------------------------------
#  Part 2: Check for and handle command-line arguments
#-------------------------------------------------------
for ARGI; do
    if [ "${ARGI}" = "--help" -o "${ARGI}" = "-h" ] ; then
	echo "launch_vehicle.sh [SWITCHES]                     "
	echo "  --help, -h                                     " 
	echo "    Display this help message                    "
	echo "  --just_make, -j                                " 
	echo "    Just make targ files, but do not launch      "
	echo "                                                 "
	echo "  --vname=<vname>                                " 
	echo "    Name of the vehicle being launched           " 
	echo "                                                 "
	echo "  --shore=<ipaddr:port>(localhost:9300)          "
	echo "    IP address and pShare port of shoreside      "
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
	exit 0
    elif [ "${ARGI//[^0-9]/}" = "$ARGI" -a "$TIME_WARP" = 1 ]; then 
        TIME_WARP=$ARGI
    elif [ "${ARGI}" = "--just_make" -o "${ARGI}" = "-j" ] ; then
	JUST_MAKE="yes"
    elif [ "${ARGI:0:8}" = "--shore=" ] ; then
	SHORE="${ARGI#--shore=*}"
    elif [ "${ARGI:0:5}" = "--ip=" ]; then
        IP_ADDR="${ARGI#--ip=*}"
    elif [ "${ARGI:0:8}" = "--mport=" ] ; then
	MOOS_PORT="${ARGI#--mport=*}"
    elif [ "${ARGI:0:9}" = "--pshare=" ] ; then
	PSHARE_PORT="${ARGI#--pshare=*}"
    elif [ "${ARGI:0:8}" = "--vname=" ] ; then
	VNAME="${ARGI#--vname=*}"
    elif [ "${ARGI}" = "--nogui" ] ; then
	GUI="no"
    else
	echo "launch_vehicle.sh: Bad Arg: " $ARGI
	exit 0
    fi
done


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
    VNAME=$FULL_VNAME  VPORT=$MOOS_PORT  PSHARE_PORT=$PSHARE_PORT  \
    START_POS=$START_POS SHORE=$SHORE IP_ADDR=$IP_ADDR

nsplug meta_vehicle.bhv targ_$FULL_VNAME.bhv -f VNAME=$FULL_VNAME  \
    START_POS=$START_POS LOITER_POS=$LOITER_POS       
   
 
if [ ${JUST_MAKE} = "yes" ] ; then
    exit 0
fi

#-------------------------------------------------------
#  Part 3: Launch the processes
#-------------------------------------------------------
printf "Launching $VNAME MOOS Community (WARP=%s) \n" $TIME_WARP
pAntler targ_$FULL_VNAME.moos >& /dev/null &

#-------------------------------------------------------
#  Part 4: Exiting and/or killing the simulation
#-------------------------------------------------------

if [ ${GUI} = "no" ] ; then
    exit 0
fi

uMAC targ_$FULL_VNAME.moos

# %1, matches the PID of the first job in the active jobs list
# namely the pAntler job(s) launched in Part 3.
echo "Killing all processes ... "
kill %1 
echo "Done killing processes.   "
