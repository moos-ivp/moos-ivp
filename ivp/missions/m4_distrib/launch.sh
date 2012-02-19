#!/bin/bash 

WARP=1
HELP="no"
JUST_BUILD="no"
BAD_ARGS=""
LAUNCH_GILDA="no"
LAUNCH_HENRY="no"
LAUNCH_ALL="yes"
LAUNCH_SHORESIDE_GLOBAL="no"

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
    if [ "${ARGI}" = "--henry" -o "${ARGI}" = "-henry" ] ; then
	LAUNCH_ALL="no"
	LAUNCH_HENRY="yes"
	UNDEFINED_ARG=""
    fi
    if [ "${ARGI}" = "--gilda" -o "${ARGI}" = "-gilda" ] ; then
	LAUNCH_ALL="no"
	LAUNCH_GILDA="yes"
	UNDEFINED_ARG=""
    fi
    if [ "${ARGI}" = "--global" -o "${ARGI}" = "-global" ] ; then
	LAUNCH_ALL="no"
	LAUNCH_SHORESIDE_GLOBAL="yes"
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
    printf "%s [SWITCHES]                                      \n" $0
    printf "Switches:                                          \n" 
    printf "  --warp=WARP_VALUE                                \n" 
    printf "  --just_build, -j                                 \n" 
    printf "  --help, -h                                       \n" 
    printf "  --henry, -henry (Launch only henry)              \n" 
    printf "  --gilda, -gilda (Launch only gilda)              \n" 
    printf "  --global, -global (Launch only shoreside_global) \n" 
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

VNAME1="henry"  # The first vehicle Community
VPORT1="9201"
VNAME2="gilda"  # The second vehicle Community
VPORT2="9202"
SNAME="shoreside"  # Shoreside Community
SPORT="9200"
START_POS1="0,0"         # Vehicle 1 Behavior configurations
LOITER_POS1="x=0,y=-75"
START_POS2="80,0"        # Vehicle 2 Behavior configurations
LOITER_POS2="x=125,y=-50"

nsplug meta_vehicle.moos targ_henry.moos -f WARP=$WARP       \
   VNAME1=$VNAME1 VNAME2=$VNAME2 VPORT1=$VPORT1 VPORT2=$VPORT2  \
   VNAME=$VNAME1 VPORT=$VPORT1 SPORT=$SPORT SNAME=$SNAME        \
   START_POS=$START_POS1

nsplug meta_vehicle.moos targ_gilda.moos -f WARP=$WARP       \
   VNAME1=$VNAME1 VNAME2=$VNAME2 VPORT1=$VPORT1 VPORT2=$VPORT2  \
   VNAME=$VNAME2 VPORT=$VPORT2 SPORT=$SPORT SNAME=$SNAME        \
   START_POS=$START_POS2

nsplug meta_shoreside_local.moos targ_shoreside_local.moos -f   \
   VNAME1=$VNAME1 VNAME2=$VNAME2 VPORT1=$VPORT1 VPORT2=$VPORT2  \
   SPORT=$SPORT SNAME=$SNAME WARP=$WARP

nsplug meta_shoreside_global.moos targ_shoreside_global.moos -f \
   WARP=$WARP

nsplug meta_vehicle.bhv targ_henry.bhv -f VNAME=$VNAME1      \
    START_POS=$START_POS1 LOITER_POS=$LOITER_POS1       

nsplug meta_vehicle.bhv targ_gilda.bhv -f VNAME=$VNAME2      \
    START_POS=$START_POS1 LOITER_POS=$LOITER_POS2       

if [ ${JUST_BUILD} = "yes" ] ; then
    exit 0
fi

#-------------------------------------------------------
#  Part 3: Launch the processes
#-------------------------------------------------------
LAUNCH_SHORESIDE_LOCAL="no"
if [ ${LAUNCH_ALL} = "yes" -o ${LAUNCH_HENRY} = "yes" ] ; then
    printf "Launching $VNAME1 MOOS Community (WARP=%s) \n" $WARP
    pAntler targ_henry.moos >& /dev/null &
    LAUNCH_SHORESIDE_LOCAL="yes"
    sleep 1
fi

if [ ${LAUNCH_ALL} = "yes" -o ${LAUNCH_GILDA} = "yes" ] ; then
    printf "Launching $VNAME2 MOOS Community (WARP=%s) \n" $WARP
    pAntler targ_gilda.moos >& /dev/null &
    LAUNCH_SHORESIDE_LOCAL="yes"
    sleep 1
fi

if [ ${LAUNCH_SHORESIDE_LOCAL} = "yes" ] ; then
    printf "Launching SHORESIDE_LOCAL MOOS Community (WARP=%s) \n"  $WARP
    pAntler targ_shoreside_local.moos >& /dev/null &
fi

if [ ${LAUNCH_SHORESIDE_GLOBAL} = "yes" ] ; then
    printf "Launching SHORESIDE_GLOBAL MOOS Community (WARP=%s) \n"  $WARP
    pAntler targ_shoreside_global.moos >& /dev/null &
fi
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
    kill %1 %2 %3 
    printf "Done killing processes.   \n"
fi


