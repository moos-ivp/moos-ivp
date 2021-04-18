#!/bin/bash -e

WARP=1
HELP="no"
JUST_BUILD="no"
SHOREONLY="no"
BAD_ARGS=""
KEY="lemon"
SURVEY_X=80
SURVEY_Y=-100
HEIGHT1=150
HEIGHT2=150
WIDTH=150
LANE_WIDTH1=20
LANE_WIDTH2=30
DEGREES1=225
COOL_FAC=100
COOL_STEP=1000

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
    if [ "${ARGI:0:8}" = "--height" ] ; then
	HEIGHT="${ARGI#--height=*}"
	UNDEFINED_ARG=""
    fi
    if [ "${ARGI:0:7}" = "--width" ] ; then
	WIDTH="${ARGI#--width=*}"
	UNDEFINED_ARG=""
    fi
    if [ "${ARGI:0:5}" = "--key" ] ; then
	KEY="${ARGI#--key=*}"
	UNDEFINED_ARG=""
    fi
    if [ "${ARGI}" = "--help" -o "${ARGI}" = "-h" ] ; then
	HELP="yes"
	UNDEFINED_ARG=""
    fi
    if [ "${ARGI}" = "--shore" -o "${ARGI}" = "-s" ] ; then
	SHOREONLY="yes"
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
    printf "%s [SWITCHES]            \n" $0
    printf "Switches:                \n" 
    printf "  --warp=WARP_VALUE      \n" 
    printf "  --height=HEIGHT_VALUE    \n" 
    printf "  --width=WIDTH_VALUE  \n" 
    printf "  --just_build, -j       \n" 
    printf "  --help, -h             \n" 
    exit 0;
fi

# Second check that the warp, height, width arguments are numerical
if [ "${WARP//[^0-9]/}" != "$WARP" ]; then 
    printf "Warp values must be numerical. Exiting now."
    exit 127
fi
if [ "${HEIGHT//[^0-9]/}" != "$HEIGHT" ]; then 
    printf "Height values must be numerical. Exiting now."
    exit 127
fi
if [ "${WIDTH//[^0-9]/}" != "$WIDTH" ]; then 
    printf "Width values must be numerical. Exiting now."
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

SNAME="shoreside"  # Shoreside Community
SPORT="9000"
SLPORT="9200"

START_POS1="0,0"         # Vehicle 1 Behavior configurations
START_POS2="200,-50"        # Vehicle 2 Behavior configurations

if [ "${SHOREONLY}" != "yes" ]; then

    nsplug meta_vehicle.moos targ_henry.moos -f WARP=$WARP      \
	VNAME=$VNAME1  VPORT=$VPORT1  LPORT=$LPORT1             \
        GROUP=$GROUP12  START_POS=$START_POS1  KEY=$KEY \
        COOL_FAC=$COOL_FAC COOL_STEPS=$COOL_STEPS
    
    nsplug meta_vehicle.bhv targ_henry.bhv -f VNAME=$VNAME1     \
	START_POS=$START_POS1 SURVEY_X=$SURVEY_X SURVEY_Y=$SURVEY_Y \
        HEIGHT=$HEIGHT1   WIDTH=$WIDTH LANE_WIDTH=$LANE_WIDTH1 DEGREES=$DEGREES1      
    
fi    

nsplug meta_shoreside.moos targ_shoreside.moos -f WARP=$WARP    \
    SLPORT=$SLPORT  SPORT=$SPORT  SNAME=$SNAME
    
    
if [ ${JUST_BUILD} = "yes" ] ; then
    exit 0
fi

#-------------------------------------------------------
#  Part 3: Launch the processes
#-------------------------------------------------------

if [ "${SHOREONLY}" != "yes" ]; then
    printf "Launching $VNAME1 MOOS Community (WARP=%s) \n" $WARP
    pAntler targ_henry.moos >& /dev/null &
    sleep 0.1
fi

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
