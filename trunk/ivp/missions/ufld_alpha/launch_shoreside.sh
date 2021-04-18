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
AUTO=""
IP_ADDR="localhost"
PSHARE_PORT="9300"
REGION="forest_lake"

#--------------------------------------------------------------
#  Part 2: Check for and handle command-line arguments
#--------------------------------------------------------------
for ARGI; do
    echo "Arg:["$ARGI"]"
    if [ "${ARGI}" = "--help" -o "${ARGI}" = "-h" ] ; then
	echo "launch_shoreside.sh [SWITCHES] [time_warp]        "
	echo "  --just_make, -j                                 " 
	echo "  --help, -h                                      " 
	echo "  --ip=<addr>       (default is localhost)        " 
	echo "  --pshare=<port>   (default is 9300)             " 
	echo "  --pavlab, -p      Set region to be MIT pavlab   " 
        echo "  --auto, -a        Auto-launched. uMAC not used. "
	exit 0;
    elif [ "${ARGI//[^0-9]/}" = "$ARGI" -a "$TIME_WARP" = 1 ]; then 
        TIME_WARP=$ARGI
    elif [ "${ARGI}" = "--just_make" -o "${ARGI}" = "-j" ] ; then
	JUST_MAKE="yes"
    elif [ "${ARGI}" = "--auto" -o "${ARGI}" = "-a" ]; then
        AUTO="yes"
    elif [ "${ARGI}" = "--pavlab" -o "${ARGI}" = "-p" ]; then
        REGION="pavlab"
    elif [ "${ARGI:0:5}" = "--ip=" ]; then
        IP_ADDR="${ARGI#--ip=*}"
    elif [ "${ARGI:0:9}" = "--pshare=" ]; then
        PSHARE_PORT="${ARGI#--pshare=*}"
    else 
	echo "launch_shoreside.sh: Bad Arg: " $ARGI
	exit 1
    fi
done

#--------------------------------------------------------------
#  Part 3: Create the .moos and .bhv files using nsplug
#--------------------------------------------------------------
# What is nsplug? Type "nsplug --help" or "nsplug --manual"

NSFLAGS="-s -f"
if [ "${AUTO}" = "" ]; then
    NSFLAGS="-i -f"
fi
nsplug meta_shoreside.moos targ_shoreside.moos $NSFLAGS WARP=$TIME_WARP  \
       IP_ADDR=$IP_ADDR  PSHARE_PORT=$PSHARE_PORT  REGION=$REGION

if [ ${JUST_MAKE} = "yes" ] ; then
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
