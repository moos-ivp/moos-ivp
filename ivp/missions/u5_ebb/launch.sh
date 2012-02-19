#!/bin/bash 

WARP=1
HELP="no"
JUST_BUILD="no"
BAD_ARGS=""

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
    printf "%s [SWITCHES]         \n" $0
    printf "Switches:             \n" 
    printf "  --warp=WARP_VALUE   \n" 
    printf "  --just_build, -j    \n" 
    printf "  --help, -h          \n" 
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

GROUP12="GROUP12"
VNAME1="henry"  # The first vehicle Community
VPORT1="9201"
LPORT1="9301"
VNAME2="gilda"  # The second vehicle Community
VPORT2="9202"
LPORT2="9302"

GROUP34="GROUP34"
VNAME3="ike"    # The third vehicle Community
VPORT3="9203"
LPORT3="9303"
VNAME4="james"  # The fourth vehicle Community
VPORT4="9204"
LPORT4="9304"

SNAME="shoreside"  # Shoreside Community
SPORT="9000"
SLPORT="9200"

START_POS1="0,0"         # Vehicle 1 Behavior configurations
LOITER_POS1="x=0,y=-75"
START_POS2="80,0"        # Vehicle 2 Behavior configurations
LOITER_POS2="x=125,y=-50"

START_POS3="25,-25"         # Vehicle 1 Behavior configurations
LOITER_POS3="x=60,y=-100"
START_POS4="105,-25"        # Vehicle 2 Behavior configurations
LOITER_POS4="x=145,y=-150"



nsplug meta_vehicle.moos targ_gilda.moos -f WARP=$WARP           \
    VNAME=$VNAME2  VPORT=$VPORT2  LPORT=$LPORT2                  \
    SPORT=$SPORT   SNAME=$SNAME   GROUP=$GROUP12                 \
    SLPORT=$SLPORT                                               \
    START_POS=$START_POS2

nsplug meta_vehicle.moos targ_henry.moos -f WARP=$WARP           \
    VNAME=$VNAME1  VPORT=$VPORT1  LPORT=$LPORT1                  \
    SPORT=$SPORT   SNAME=$SNAME   GROUP=$GROUP12                 \
    SLPORT=$SLPORT                                               \
    START_POS=$START_POS1

nsplug meta_vehicle.moos targ_ike.moos -f WARP=$WARP             \
    VNAME=$VNAME3  VPORT=$VPORT3  LPORT=$LPORT3                  \
    SPORT=$SPORT   SNAME=$SNAME   GROUP=$GROUP34                 \
    SLPORT=$SLPORT                                               \
    START_POS=$START_POS3

nsplug meta_vehicle.moos targ_james.moos -f WARP=$WARP           \
    VNAME=$VNAME4  VPORT=$VPORT4  LPORT=$LPORT4                  \
    SPORT=$SPORT   SNAME=$SNAME   GROUP=$GROUP34                 \
    SLPORT=$SLPORT                                               \
    START_POS=$START_POS4

nsplug meta_shoreside.moos targ_shoreside.moos -f WARP=$WARP     \
    SLPORT=$SLPORT                                               \
    SPORT=$SPORT SNAME=$SNAME


nsplug meta_vehicle.bhv targ_henry.bhv -f VNAME=$VNAME1      \
    START_POS=$START_POS1 LOITER_POS=$LOITER_POS1       

nsplug meta_vehicle.bhv targ_gilda.bhv -f VNAME=$VNAME2      \
    START_POS=$START_POS1 LOITER_POS=$LOITER_POS2       

nsplug meta_vehicle.bhv targ_ike.bhv -f   VNAME=$VNAME3      \
    START_POS=$START_POS3 LOITER_POS=$LOITER_POS3       

nsplug meta_vehicle.bhv targ_james.bhv -f VNAME=$VNAME4      \
    START_POS=$START_POS4 LOITER_POS=$LOITER_POS4       

if [ ${JUST_BUILD} = "yes" ] ; then
    exit 0
fi

#-------------------------------------------------------
#  Part 3: Launch the processes
#-------------------------------------------------------

printf "Launching $VNAME1 MOOS Community (WARP=%s) \n" $WARP
pAntler targ_henry.moos >& /dev/null &
#sleep 0.4
#printf "Launching $VNAME2 MOOS Community (WARP=%s) \n" $WARP
#pAntler targ_gilda.moos >& /dev/null &
#sleep 0.4

#printf "Launching $VNAME3 MOOS Community (WARP=%s) \n" $WARP
#pAntler targ_ike.moos >& /dev/null &
#sleep 0.4
#printf "Launching $VNAME4 MOOS Community (WARP=%s) \n" $WARP
#pAntler targ_james.moos >& /dev/null &
#sleep 0.4


printf "Launching $SNAME MOOS Community (WARP=%s) \n"  $WARP
pAntler targ_shoreside.moos >& /dev/null &
printf "Done \n"

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
    kill %1 %2 %3 %4 %5
    printf "Done killing processes.   \n"
fi


