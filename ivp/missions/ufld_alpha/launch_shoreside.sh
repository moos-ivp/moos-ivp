#!/bin/bash -e
#--------------------------------------------------------------
#   Script: launch_shoreside.sh                                    
#   Author: Michael Benjamin  
#     Date: April 2020     
#--------------------------------------------------------------  
#  Part 1: Declare global var defaults
#--------------------------------------------------------------
TIME_WARP=1
JUST_MAKE="no"

IP_ADDR="localhost"
MOOS_PORT="9000"

AUTO=""
PSHARE_PORT="9200"

REGION="forest_lake"

#--------------------------------------------------------------
#  Part 2: Check for and handle command-line arguments
#--------------------------------------------------------------
for ARGI; do
    if [ "${ARGI}" = "--help" -o "${ARGI}" = "-h" ]; then
	echo "launch_shoreside.sh [SWITCHES] [time_warp]        "
	echo "  --help, -h                                      " 
	echo "    Display this help message                     "
	echo "  --just_make, -j                                 " 
	echo "    Just make targ files, but do not launch       "
	echo "                                                  "
	echo "  --mport=<port>                                  "
	echo "    Port number of this vehicle's MOOSDB port     "
	echo "                                                  "
	echo "  --pshare=<port>                                 " 
	echo "    Port number of this vehicle's pShare port     "
	echo "                                                  "
	echo "  --ip=<ipaddr>       (default is localhost)      " 
	echo "                                                  "
	echo "  --ip=<ipaddr>                                   " 
	echo "    Force pHostInfo to use this IP Address        "
	echo "                                                  "
	echo "  --pavlab, -p      Set region to be MIT pavlab   " 
        echo "  --auto, -a        Auto-launched. uMAC not used. "
	exit 0;
    elif [ "${ARGI//[^0-9]/}" = "$ARGI" -a "$TIME_WARP" = 1 ]; then 
        TIME_WARP=$ARGI
    elif [ "${ARGI}" = "--just_make" -o "${ARGI}" = "-j" ] ; then
	JUST_MAKE="yes"
    elif [ "${ARGI:0:5}" = "--ip=" ]; then
        IP_ADDR="${ARGI#--ip=*}"
    elif [ "${ARGI:0:7}" = "--mport" ]; then
	MOOS_PORT="${ARGI#--mport=*}"
    elif [ "${ARGI:0:9}" = "--pshare=" ]; then
        PSHARE_PORT="${ARGI#--pshare=*}"

    elif [ "${ARGI}" = "--auto" -o "${ARGI}" = "-a" ]; then
        AUTO="yes"
    elif [ "${ARGI}" = "--pavlab" -o "${ARGI}" = "-p" ]; then
        REGION="pavlab"
    else 
	echo "launch_shoreside.sh: Bad Arg: " $ARGI "Exit Code 1."
	exit 1
    fi
done

#--------------------------------------------------------------
#  Part 3: Create the .moos and .bhv files using nsplug
#--------------------------------------------------------------
nsplug meta_shoreside.moos targ_shoreside.moos -i -f WARP=$TIME_WARP  \
       IP_ADDR=$IP_ADDR  PSHARE_PORT=$PSHARE_PORT                     \
       REGION=$REGION    MOOS_PORT=$MOOS_PORT

if [ ${JUST_MAKE} = "yes" ]; then
    echo "Files assembled; nothing launched; exiting per request."
    exit 0
fi

#--------------------------------------------------------------
#  Part 4: Launch the processes
#--------------------------------------------------------------
echo "Launching $VNAME MOOS Community WARP:" $TIME_WARP
pAntler targ_shoreside.moos >& /dev/null &
echo "Done launnching shoreside"

#-------------------------------------------------------------- 
#  Part 5: Unless auto-launched, launch uMAC until mission quit          
#-------------------------------------------------------------- 
if [ "${AUTO}" = "" ]; then
    uMAC targ_shoreside.moos
    kill -- -$$
fi
