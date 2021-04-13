#!/bin/bash 
#-------------------------------------------------------
#   Script: launch_shoreside.sh                       
#  Mission: lab_10_baseline
#-------------------------------------------------------
#  Part 1: Set global var defaults
#----------------------------------------------------------
TIME_WARP=1
JUST_MAKE="no"

IP_ADDR="localhost"
PSHARE_PORT="9200"

MOOS_PORT="9000"
CONF="yes"

#-------------------------------------------------------
#  Part 2: Check for and handle command-line arguments
#-------------------------------------------------------
for ARGI; do
    if [ "${ARGI}" = "--help" -o "${ARGI}" = "-h" ]; then
	echo "launch_shoreside.sh [SWITCHES] [WARP]         "
	echo "  --help, -h                                  " 
	echo "    Display this help message                 "
	echo "  --just_make, -j                             " 
	echo "    Just make targ files, but do not launch   "
	echo "                                              "
	echo "  --mport=<port>                              "
	echo "    Port number of this vehicle's MOOSDB port "
	echo "                                              "
	echo "  --pshare=<port>                             " 
	echo "    Port number of this vehicle's pShare port "
	echo "                                              "
	echo "  --ip=<ipaddr>                               " 
	echo "    Force pHostInfo to use this IP Address    "
	echo "  --nc                                        " 
	echo "    No confirmation before launching          "
	exit 0;
    elif [ "${ARGI//[^0-9]/}" = "$ARGI" ]; then 
        TIME_WARP=$ARGI
    elif [ "${ARGI}" = "--just_build" -o "${ARGI}" = "-j" ]; then
	JUST_MAKE="yes"
    elif [ "${ARGI}" = "-nc" -o "${ARGI}" = "--nc" ]; then
	CONF="no"
    elif [ "${ARGI:0:5}" = "--ip=" ]; then
        IP_ADDR="${ARGI#--ip=*}"
    elif [ "${ARGI:0:7}" = "--mport" ]; then
	MOOS_PORT="${ARGI#--mport=*}"
    elif [ "${ARGI:0:9}" = "--pshare=" ]; then
        PSHARE_PORT="${ARGI#--pshare=*}"
    else
	echo "launch_shoreside.sh Bad Arg:" $ARGI
	exit 1
    fi
done


if [ "${CONF}" = "yes" ]; then 
    echo "PSHARE_PORT = [${PSHARE_PORT}]"
    echo "MOOS_PORT =   [${MOOS_PORT}]"
    echo "IP_ADDR =     [${IP_ADDR}]"
    echo "TIME_WARP =   [${TIME_WARP}]"
    echo -n "Hit any key to continue with launching"
    read ANSWER
fi


#-------------------------------------------------------
#  Part 3: Create the .moos and .bhv files. 
#-------------------------------------------------------
nsplug meta_shoreside.moos targ_shoreside.moos -f WARP=$TIME_WARP  \
       IP_ADDR=$IP_ADDR       PSHARE_PORT=$PSHARE_PORT             \
       MOOS_PORT=$MOOS_PORT  

if [ ${JUST_MAKE} = "yes" ]; then
    echo "Files assembled; nothing launched; exiting per request."
    exit 0
fi

#-------------------------------------------------------
#  Part 4: Launch the processes
#-------------------------------------------------------
echo "Launching shoreside MOOS Community (WARP=%s) " $TIME_WARP
pAntler targ_shoreside.moos >& /dev/null &

uMAC targ_shoreside.moos

#-------------------------------------------------------
#  Part 5: Exiting and/or killing the simulation
#-------------------------------------------------------
kill -- -$$
