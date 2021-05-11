#!/bin/bash -e 
#--------------------------------------------------------------  
#  Part 1: Declare global var defaults
#--------------------------------------------------------------
ME=`basename "$0"`
TIME_WARP=1
JUST_MAKE="no"
VERBOSE=""
CONFIRM="yes"
CMD_ARGS=""

IP_ADDR="localhost"
MOOS_PORT="9000"
PSHARE_PORT="9200"

#-------------------------------------------------------
#  Part 2: Check for and handle command-line arguments
#-------------------------------------------------------
for ARGI; do
    if [ "${ARGI}" = "--help" -o "${ARGI}" = "-h" ]; then
	echo "$ME [SWITCHES] [WARP]                         "
	echo "  --help, -h                                  " 
	echo "    Display this help message                 "
	echo "  --just_make, -j                             " 
	echo "    Just make targ files, but do not launch   "
        echo "  --verbose, -v                               "
        echo "    Increase verbosity                        "
	echo "  --noconfirm, -nc                            " 
	echo "    No confirmation before launching          "
	echo "                                              "
	echo "  --ip=<localhost>                            " 
	echo "    Force pHostInfo to use this IP Address    "
	echo "  --mport=<9000>                              "
	echo "    Port number of this vehicle's MOOSDB port "
	echo "  --pshare=<9200>                             " 
	echo "    Port number of this vehicle's pShare port "
	exit 0;
    elif [ "${ARGI//[^0-9]/}" = "$ARGI" -a "$TIME_WARP" = 1 ]; then 
        TIME_WARP=$ARGI
    elif [ "${ARGI}" = "--just_make" -o "${ARGI}" = "-j" ]; then
	JUST_MAKE="yes"
    elif [ "${ARGI}" = "--verbose" -o "${ARGI}" = "-v" ]; then
	VERBOSE="yes"
    elif [ "${ARGI}" = "--noconfirm" -o "${ARGI}" = "-nc" ]; then
	CONFIRM="no"

    elif [ "${ARGI:0:5}" = "--ip=" ]; then
        IP_ADDR="${ARGI#--ip=*}"
    elif [ "${ARGI:0:7}" = "--mport" ]; then
	MOOS_PORT="${ARGI#--mport=*}"
    elif [ "${ARGI:0:9}" = "--pshare=" ]; then
        PSHARE_PORT="${ARGI#--pshare=*}"
    else 
	echo "$ME Bad Arg:" $ARGI "Exit Code 1"
	exit 1
    fi
done


if [ "${VERBOSE}" = "yes" -o "${CONFIRM}" = "yes" ]; then 
    echo "$ME"
    echo "CMD_ARGS =    [${CMD_ARGS}]"
    echo "TIME_WARP =   [${TIME_WARP}]"
    echo "IP_ADDR =     [${IP_ADDR}]"
    echo "MOOS_PORT =   [${MOOS_PORT}]"
    echo "PSHARE_PORT = [${PSHARE_PORT}]"
    echo -n "Hit any key to continue with launching"
    read ANSWER
fi


#-------------------------------------------------------
#  Part 3: Create the .moos file(s)
#-------------------------------------------------------
nsplug meta_shoreside.moos targ_shoreside.moos -f WARP=$TIME_WARP \
       IP_ADDR=$IP_ADDR  PSHARE_PORT=$PSHARE_PORT
       MOOS_PORT=$MOOS_PORT

if [ ${JUST_MAKE} = "yes" ]; then
    echo "Files assembled; nothing launched; exiting per request."
    exit 0
fi

#-------------------------------------------------------
#  Part 4: Launch the processes
#-------------------------------------------------------
echo "Launching Shoreside MOOS Community with WARP:" $TIME_WARP
pAntler targ_shoreside.moos >& /dev/null &
echo "Done "

uMAC targ_shoreside.moos

echo "Killing all processes ... "
kill -- -$$
echo "Done killing processes.   "


