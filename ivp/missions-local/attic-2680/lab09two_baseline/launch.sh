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
    if [ "${ARGI}" = "--just_build" ] ; then
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
    printf "  --just_build        \n" 
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

VNAME1="archie"  # The first vehicle Community
VPORT1="9201"
LPORT1="9301"

VNAME9="betty"
VPORT9="9209"
LPORT9="9309"

SNAME="shoreside"  # Shoreside Community
SPORT="9000"
SLPORT="9200"

START_POS1="0,0"   # Vehicle 1 Behavior configurations
START_POS9="0,0"   # Vehicle 9 UUV configuration

nsplug meta_archie.moos targ_archie.moos -f WARP=$WARP       \
   VNAME=$VNAME1 VPORT=$VPORT1 LPORT=$LPORT1                 \
   START_POS=$START_POS1

nsplug meta_betty.moos targ_betty.moos -f WARP=$WARP         \
   VNAME=$VNAME9 VPORT=$VPORT9 LPORT=$LPORT9                 \
   START_POS=$START_POS9

nsplug meta_shoreside.moos targ_shoreside.moos -f WARP=$WARP \
   SPORT=$SPORT SNAME=$SNAME SLPORT=$SLPORT


nsplug meta_archie.bhv targ_archie.bhv -f VNAME=$VNAME1      \
    START_POS=$START_POS1 

nsplug meta_betty.bhv targ_betty.bhv -f VNAME=$VNAME9        \
    START_POS=$START_POS9 


if [ ${JUST_BUILD} = "yes" ] ; then
    exit 0
fi

#-------------------------------------------------------
#  Part 3: Launch the processes
#-------------------------------------------------------

printf "Launching $VNAME1 MOOS Community (WARP=%s) \n" $WARP
pAntler targ_archie.moos >& /dev/null &
sleep 1
printf "Launching $VNAME9 MOOS Community (WARP=%s) \n" $WARP
pAntler targ_betty.moos >& /dev/null &
sleep 1

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
    kill %1 %2 %3 
    printf "Done killing processes.   \n"
fi


