#!/bin/bash 

WARP=1
HELP="no"
JUST_BUILD="no"
BAD_ARGS=""
HOSTNAME=$(hostname -s)
VNAME=$(id -un)
MOOS_PORT="9201"
UDP_LISTEN_PORT="9301"

#-------------------------------------------------------
#  Part 1: Check for and handle command-line arguments
#-------------------------------------------------------
let COUNT=0
for ARGI; do
    UNDEFINED_ARG=$ARGI
    if [ "${ARGI:0:6}" = "--warp" ] ; then
	WARP="${ARGI#--warp=*}"
	UNDEFINED_ARG=""
    fi
    if [ "${ARGI:0:7}" = "--shore" ] ; then
	SHOREIP="${ARGI#--shore=*}"
	UNDEFINED_ARG=""
    fi
    if [ "${ARGI:0:7}" = "--mport" ] ; then
	MOOS_PORT="${ARGI#--mport=*}"
	UNDEFINED_ARG=""
    fi
    if [ "${ARGI:0:7}" = "--lport" ] ; then
	UDP_LISTEN_PORT="${ARGI#--lport=*}"
	UNDEFINED_ARG=""
    fi
    if [ "${ARGI:0:7}" = "--vname" ] ; then
	VNAME="${ARGI#--vname=*}"
	UNDEFINED_ARG=""
    fi
    if [ "${ARGI}" = "--help" -o "${ARGI}" = "-h" ] ; then
	HELP="yes"
	UNDEFINED_ARG=""
    fi
    # Handle Warp shortcut
    if [ "${ARGI//[^0-9]/}" = "$ARGI" -a "$COUNT" = 0 ]; then 
        WARP=$ARGI
        let "COUNT=$COUNT+1"
        UNDEFINED_ARG=""
    fi
    if [ "${ARGI}" = "--just_build" -o "${ARGI}" = "-j" ] ; then
	JUST_BUILD="yes"
	UNDEFINED_ARG=""
    fi
    if [ "${UNDEFINED_ARG}" != "" ] ; then
	BAD_ARGS=$UNDEFINED_ARG
    fi
done

if [ "${BAD_ARGS}" != "" ] ; then
    printf "Bad Argument: %s \n" $BAD_ARGS
    exit 0
fi

if [ "${HELP}" = "yes" ]; then
    printf "%s [SWITCHES]                           \n" $0
    printf "Switches:                               \n" 
    printf "  --warp=WARP_VALUE                     \n" 
    printf "  --vname=VEHICLE_NAME                  \n" 
    printf "  --mport=MOOSDB Port #                 \n" 
    printf "  --lport=MOOSBridge UDPListen Port #   \n" 
    printf "  --just_build, -j                      \n" 
    printf "  --help, -h                            \n" 
    exit 0;
fi

# Second check that the warp argument is numerical
if [ "${WARP//[^0-9]/}" != "$WARP" ]; then 
    printf "Warp values must be numerical. Exiting now."
    exit 127
fi

#-------------------------------------------------------
#  Part 2: Create the .moos and .bhv files. 
#-------------------------------------------------------

GROUP="GROUP12"
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

#START_POS="0,0"         
#LOITER_POS="x=0,y=-75"

nsplug meta_vehicle.moos targ_vehicle.moos -f WARP=$WARP                \
    VNAME=$FULL_VNAME  VPORT=$MOOS_PORT  LPORT=$UDP_LISTEN_PORT         \
    GROUP=$GROUP START_POS=$START_POS SHOREIP=$SHOREIP

nsplug meta_vehicle.bhv targ_$FULL_VNAME.bhv -f VNAME=$FULL_VNAME       \
    START_POS=$START_POS LOITER_POS=$LOITER_POS       
    
if [ ${JUST_BUILD} = "yes" ] ; then
    exit 0
fi

#-------------------------------------------------------
#  Part 3: Launch the processes
#-------------------------------------------------------

printf "Launching $VNAME MOOS Community (WARP=%s) \n" $WARP
pAntler targ_vehicle.moos >& /dev/null &

#-------------------------------------------------------
#  Part 4: Exiting and/or killing the simulation
#-------------------------------------------------------

ANSWER="0"
while [ "${ANSWER}" != "1" -a "${ANSWER}" != "2" ]; do
    printf "Now what? (1) Exit script (2) Exit and Kill Simulation \n"
    printf "> "
    read ANSWER
done

# %1, %2, %3 matches the PID of the first three jobs in the active
# jobs list, namely the three pAntler jobs launched in Part 3.
if [ "${ANSWER}" = "2" ]; then
    printf "Killing all processes ... \n"
    kill %1 
    printf "Done killing processes.   \n"
fi
