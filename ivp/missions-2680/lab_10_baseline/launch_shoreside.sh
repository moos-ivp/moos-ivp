#!/bin/bash 
#-------------------------------------------------------
#   Script: launch_shoreside.sh                       
#  Mission: lab_10_baseline
#-------------------------------------------------------

TIME_WARP=1
JUST_MAKE="no"
IP_ADDR=""
PSHARE_PORT="9300"
MOOS_PORT="9000"

#-------------------------------------------------------
#  Part 1: Check for and handle command-line arguments
#-------------------------------------------------------
for ARGI; do
    if [ "${ARGI}" = "--help" -o "${ARGI}" = "-h" ] ; then
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
	exit 0;
    elif [ "${ARGI//[^0-9]/}" = "$ARGI" ]; then 
        TIME_WARP=$ARGI
    elif [ "${ARGI}" = "--just_build" -o "${ARGI}" = "-j" ] ; then
	JUST_MAKE="yes"
    elif [ "${ARGI:0:5}" = "--ip=" ]; then
        IP_ADDR="${ARGI#--ip=*}"
    elif [ "${ARGI:0:7}" = "--mport" ] ; then
	MOOS_PORT="${ARGI#--mport=*}"
    elif [ "${ARGI:0:9}" = "--pshare=" ]; then
        PSHARE_PORT="${ARGI#--pshare=*}"
    else
	echo "launch_shoreside.sh Bad Arg:" $ARGI
	exit 0
    fi
done

#-------------------------------------------------------
#  Part 2: Create the .moos and .bhv files. 
#-------------------------------------------------------

nsplug meta_shoreside.moos targ_shoreside.moos -f WARP=$TIME_WARP  \
       IP_ADDR=$IP_ADDR       PSHARE_PORT=$PSHARE_PORT             \
       MOOS_PORT=$MOOS_PORT  

if [ ${JUST_MAKE} = "yes" ] ; then
    exit 0
fi

#-------------------------------------------------------
#  Part 3: Launch the processes
#-------------------------------------------------------
echo "Launching shoreside MOOS Community (WARP=%s) \n" $TIME_WARP
pAntler targ_shoreside.moos >& /dev/null &


#-------------------------------------------------------
#  Part 4: Exiting and/or killing the simulation
#-------------------------------------------------------
uMAC targ_shoreside.moos

# %1, matches the PID of the first job in the active jobs list
# namely the pAntler job(s) launched in Part 3.
echo "Killing all processes ... "
kill %1 
echo "Done killing processes.   "
