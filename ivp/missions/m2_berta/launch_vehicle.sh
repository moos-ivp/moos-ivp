#!/bin/bash -e
#-------------------------------------------------------------- 
#   Script: launch_vehicle.sh
#  Mission: m2_berta
#   Author: M.Benjamin
#   LastEd: May 2024
#-------------------------------------------------------------- 
#  Part 1: A convenience function for producing terminal
#          debugging/status output depending on verbosity.
#-------------------------------------------------------------- 
vecho() { if [ "$VERBOSE" != "" ]; then echo "$ME: $1"; fi }

#--------------------------------------------------------------
#  Part 2: Declare global var defaults
#--------------------------------------------------------------
ME=`basename "$0"`
TIME_WARP=1
JUST_MAKE=""
VERBOSE=""
AUTO_LAUNCHED="no"
CMD_ARGS=""

IP_ADDR="localhost"
MOOS_PORT="9001"
PSHARE_PORT="9201"
SHORE_IP="localhost"
SHORE_PSHARE="9200"

VNAME="abe"
COLOR="yellow"
XMODE="M300"
START_POS="0,0"

LOITER_POS="0,-50"  

#------------------------------------------------------------
#  Part 3: Check for and handle command-line arguments
#------------------------------------------------------------
for ARGI; do
    CMD_ARGS+=" ${ARGI}"
    if [ "${ARGI}" = "--help" -o "${ARGI}" = "-h" ]; then
	echo "$ME [OPTIONS] [time_warp]                      "
	echo "                                               " 
	echo "Options:                                       "
	echo "  --help, -h         Show this help message    " 
	echo "  --just_make, -j    Only create targ files    " 
	echo "  --verbose, -v      Verbose, confirm launch   "
	echo "                                               "
        echo "  --auto, -a                                   "
        echo "    Auto-launched by a script.                 "
        echo "    Will not launch uMAC as the final step.    "
	echo "                                               "
	echo "  --ip=<localhost>                             " 
	echo "    Force pHostInfo to use this IP Address     "
	echo "  --mport=<9001>                               "
	echo "    Port number of this vehicle's MOOSDB port  "
	echo "  --pshare=<9201>                              " 
	echo "    Port number of this vehicle's pShare port  "
	echo "  --shore=<localhost>                          " 
	echo "    IP address location of shoreside           "
	echo "  --shore_pshare=<9200>                        " 
	echo "    Shoreside pShare is listening port         "
	echo "                                               "
	echo "  --vname=<abe>                                " 
	echo "    Name of the vehicle being launched         " 
	echo "  --color=<yellow>                             " 
	echo "    Color of the vehicle being launched        "
	echo "  --sim, -s                                    "
	echo "    This is simultion not robot                "
	echo "  --start=<X,Y>      (default is 0,0)          " 
	echo "    Start position chosen by script launching  "
	echo "    this script (to ensure separation)         "
	echo "                                               "
	echo "  --loi_pos=<X,Y>     (default is 0,0)         " 
	exit 0;
    elif [ "${ARGI//[^0-9]/}" = "$ARGI" -a "$TIME_WARP" = 1 ]; then 
        TIME_WARP=$ARGI
    elif [ "${ARGI}" = "--just_make" -o "${ARGI}" = "-j" ]; then
	JUST_MAKE="yes"
    elif [ "${ARGI}" = "--verbose" -o "${ARGI}" = "-v" ]; then
        VERBOSE="yes"
    elif [ "${ARGI}" = "--auto" -o "${ARGI}" = "-a" ]; then
        AUTO_LAUNCHED="yes" 

    elif [ "${ARGI:0:5}" = "--ip=" ]; then
        IP_ADDR="${ARGI#--ip=*}"
    elif [ "${ARGI:0:7}" = "--mport" ]; then
	MOOS_PORT="${ARGI#--mport=*}"
    elif [ "${ARGI:0:9}" = "--pshare=" ]; then
        PSHARE_PORT="${ARGI#--pshare=*}"
    elif [ "${ARGI:0:8}" = "--shore=" ]; then
        SHORE_IP="${ARGI#--shore=*}"
    elif [ "${ARGI:0:15}" = "--shore_pshare=" ]; then
        SHORE_PSHARE="${ARGI#--shore_pshare=*}"

    elif [ "${ARGI:0:8}" = "--vname=" ]; then
        VNAME="${ARGI#--vname=*}"
    elif [ "${ARGI:0:8}" = "--color=" ]; then
        COLOR="${ARGI#--color=*}"
    elif [ "${ARGI}" = "--sim" -o "${ARGI}" = "-s" ]; then
        XMODE="SIM"
    elif [ "${ARGI:0:8}" = "--start=" ]; then
        START_POS="${ARGI#--start=*}"

    elif [ "${ARGI:0:9}" = "--loipos=" ]; then
        LOITER_POS="${ARGI#--loipos=*}"

    else 
	echo "$ME: Bad Arg:[$ARGI]. Exit Code 1."
	exit 1
    fi
done

#--------------------------------------------------------------
#  Part 4: If Heron hardware, set key info based on IP address
#--------------------------------------------------------------
if [ "${XMODE}" = "M300" ]; then
    COLOR=`get_heron_info.sh --color --hint=$COLOR`
    IP_ADDR=`get_heron_info.sh --ip`
    FSEAT_IP=`get_heron_info.sh --fseat`
    VNAME=`get_heron_info.sh --name`
    if [ $? != 0 ]; then
	echo "$ME: Problem getting Heron Info. Exit Code 2"
	exit 2
    fi
fi
     
#---------------------------------------------------------------
#  Part 5: If verbose, show vars and confirm before launching
#---------------------------------------------------------------
if [ "${VERBOSE}" = "yes" ]; then 
    echo "============================================"
    echo "     launch_vehicle.sh SUMMARY        $VNAME"
    echo "============================================"
    echo "$ME"
    echo "CMD_ARGS =      [${CMD_ARGS}]     "
    echo "TIME_WARP =     [${TIME_WARP}]    "
    echo "JUST_MAKE =     [${JUST_MAKE}]    "
    echo "AUTO_LAUNCHED = [${AUTO_LAUNCHED}]"
    echo "----------------------------------"
    echo "IP_ADDR =       [${IP_ADDR}]      "
    echo "MOOS_PORT =     [${MOOS_PORT}]    "
    echo "PSHARE_PORT =   [${PSHARE_PORT}]  "
    echo "SHORE_IP =      [${SHORE_IP}]     "
    echo "SHORE_PSHARE =  [${SHORE_PSHARE}] "
    echo "----------------------------------"
    echo "VNAME =         [${VNAME}]        "
    echo "COLOR =         [${COLOR}]        "
    echo "XMODE =         [${XMODE}]        "
    echo "------------Sim-------------------"
    echo "START_POS =     [${START_POS}]    "
    echo "------------Fld-------------------"
    echo "FSEAT_IP =      [${FSEAT_IP}]     "
    echo "------------Custom----------------"
    echo "LOITER_POS =    [${LOITER_POS}]   "
    echo "                                  "
    echo -n "Hit any key to continue launching $VNAME "
    read ANSWER
fi

#--------------------------------------------------------------
#  Part 6: Create the .moos and .bhv files. 
#--------------------------------------------------------------
NSFLAGS="--strict --force"
if [ "${AUTO_LAUNCHED}" = "no" ]; then
    NSFLAGS="--interactive --force"
fi

nsplug meta_vehicle.moos targ_$VNAME.moos $NSFLAGS WARP=$TIME_WARP \
       MOOS_PORT=$MOOS_PORT         PSHARE_PORT=$PSHARE_PORT  \
       IP_ADDR=$IP_ADDR             SHORE_IP=$SHORE_IP        \
       SHORE_PSHARE=$SHORE_PSHARE   VNAME=$VNAME              \
       COLOR=$COLOR                 START_POS=$START_POS      \
       FSEAT_IP=$FSEAT_IP           XMODE=$XMODE

nsplug meta_vehicle.bhv targ_$VNAME.bhv $NSFLAGS VNAME=$VNAME \
       START_POS=$START_POS                                   \
       LOITER_POS=$LOITER_POS

if [ "${JUST_MAKE}" = "yes" ]; then
    echo "Targ files made; exiting without launch."
    exit 0
fi

#--------------------------------------------------------------
#  Part 7: Launch the vehicle mission
#--------------------------------------------------------------
echo "Launching $VNAME MOOS Community. WARP="$TIME_WARP
pAntler targ_$VNAME.moos >& /dev/null &
echo "Done Launching $VNAME MOOS Community"

#--------------------------------------------------------------
#  Part 8: If launched from script, we're done, exit now
#--------------------------------------------------------------
if [ "${AUTO_LAUNCHED}" = "yes" ]; then
    exit 0
fi

#--------------------------------------------------------------
# Part 9: Launch uMAC until the mission is quit
#--------------------------------------------------------------
uMAC targ_$VNAME.moos
kill -- -$$
