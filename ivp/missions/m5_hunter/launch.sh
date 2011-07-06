#!/bin/sh 

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
VNAME2="betty"  # The second vehicle Community
VPORT2="9202"
VNAME3="charlie"  # The second vehicle Community
VPORT3="9203"
VNAME4="david"  # The third vehicle Community
VPORT4="9204"
VNAME5="ernie"  # The fifth vehicle Community
VPORT5="9205"

VNAME9="prey"
VPORT9="9209"

SNAME="shoreside"  # Shoreside Community
SPORT="9200"
START_POS1="0,0"         # Vehicle 1 Behavior configurations
LOITER_POS1="x=-90,y=-130"
START_POS2="20,0"        # Vehicle 2 Behavior configurations
LOITER_POS2="x=-40,y=-340"
START_POS3="40,0"        # Vehicle 3 Behavior configurations
LOITER_POS3="x=210,y=-390"
START_POS4="60,0"        # Vehicle 4 Behavior configurations
LOITER_POS4="x=410,y=-200"
START_POS5="80,0"        # Vehicle 5 Behavior configurations
LOITER_POS5="x=220,y=-20"

START_POS9="-180,-100"   # Vehicle 9 UUV configuration

#START_POS1="0,0"         # Vehicle 1 Behavior configurations
#LOITER_POS1="x=0,y=-75"
#START_POS2="20,0"        # Vehicle 2 Behavior configurations
#LOITER_POS2="x=25,y=-50"
#START_POS3="40,0"        # Vehicle 3 Behavior configurations
#LOITER_POS3="x=50,y=-75"
#START_POS4="60,0"        # Vehicle 4 Behavior configurations
#LOITER_POS4="x=75,y=-50"
#START_POS5="80,0"        # Vehicle 5 Behavior configurations
#LOITER_POS5="x=100,y=-75"


nsplug meta_vehicle.moos targ_archie.moos -f WARP=$WARP       \
   VNAME1=$VNAME1 VNAME2=$VNAME2 VNAME3=$VNAME3 VNAME4=$VNAME4 VNAME5=$VNAME5  \
   VPORT1=$VPORT1 VPORT2=$VPORT2 VPORT3=$VPORT3 VPORT4=$VPORT4 VPORT5=$VPORT5  \
   VNAME=$VNAME1 VPORT=$VPORT1 SPORT=$SPORT SNAME=$SNAME VTYPE=KAYAK       \
   START_POS=$START_POS1

nsplug meta_vehicle.moos targ_betty.moos -f WARP=$WARP       \
   VNAME1=$VNAME1 VNAME2=$VNAME2 VNAME3=$VNAME3 VNAME4=$VNAME4 VNAME5=$VNAME5  \
   VPORT1=$VPORT1 VPORT2=$VPORT2 VPORT3=$VPORT3 VPORT4=$VPORT4 VPORT5=$VPORT5  \
   VNAME=$VNAME2 VPORT=$VPORT2 SPORT=$SPORT SNAME=$SNAME VTYPE=KAYAK       \
   START_POS=$START_POS2

nsplug meta_vehicle.moos targ_charlie.moos -f WARP=$WARP       \
   VNAME1=$VNAME1 VNAME2=$VNAME2 VNAME3=$VNAME3 VNAME4=$VNAME4 VNAME5=$VNAME5  \
   VPORT1=$VPORT1 VPORT2=$VPORT2 VPORT3=$VPORT3 VPORT4=$VPORT4 VPORT5=$VPORT5  \
   VNAME=$VNAME3 VPORT=$VPORT3 SPORT=$SPORT SNAME=$SNAME VTYPE=KAYAK       \
   START_POS=$START_POS3

nsplug meta_vehicle.moos targ_david.moos -f WARP=$WARP       \
   VNAME1=$VNAME1 VNAME2=$VNAME2 VNAME3=$VNAME3 VNAME4=$VNAME4 VNAME5=$VNAME5  \
   VPORT1=$VPORT1 VPORT2=$VPORT2 VPORT3=$VPORT3 VPORT4=$VPORT4 VPORT5=$VPORT5  \
   VNAME=$VNAME4 VPORT=$VPORT4 SPORT=$SPORT SNAME=$SNAME VTYPE=KAYAK       \
   START_POS=$START_POS4

nsplug meta_vehicle.moos targ_ernie.moos -f WARP=$WARP       \
   VNAME1=$VNAME1 VNAME2=$VNAME2 VNAME3=$VNAME3 VNAME4=$VNAME4 VNAME5=$VNAME5  \
   VPORT1=$VPORT1 VPORT2=$VPORT2 VPORT3=$VPORT3 VPORT4=$VPORT4 VPORT5=$VPORT5  \
   VNAME=$VNAME5 VPORT=$VPORT5 SPORT=$SPORT SNAME=$SNAME VTYPE=KAYAK       \
   START_POS=$START_POS5

nsplug meta_uuv.moos targ_prey.moos -f WARP=$WARP       \
   VNAME1=$VNAME1 VNAME2=$VNAME2 VNAME3=$VNAME3 VNAME4=$VNAME4 VNAME5=$VNAME5  \
   VPORT1=$VPORT1 VPORT2=$VPORT2 VPORT3=$VPORT3 VPORT4=$VPORT4 VPORT5=$VPORT5  \
   VNAME=$VNAME9 VPORT=$VPORT9 SPORT=$SPORT SNAME=$SNAME VTYPE=UUV       \
   START_POS=$START_POS9

nsplug meta_shoreside.moos targ_shoreside.moos -f WARP=$WARP    \
   VNAME1=$VNAME1 VNAME2=$VNAME2 VNAME3=$VNAME3 VNAME4=$VNAME4 VNAME5=$VNAME5 VNAME9=$VNAME9 \
   VPORT1=$VPORT1 VPORT2=$VPORT2 VPORT3=$VPORT3 VPORT4=$VPORT4 VPORT5=$VPORT5 VPORT9=$VPORT9 \
   SPORT=$SPORT SNAME=$SNAME




nsplug meta_vehicle.bhv targ_archie.bhv -f VNAME=$VNAME1      \
    START_POS=$START_POS1 LOITER_POS=$LOITER_POS1       

nsplug meta_vehicle.bhv targ_betty.bhv -f VNAME=$VNAME2      \
    START_POS=$START_POS2 LOITER_POS=$LOITER_POS2       

nsplug meta_vehicle.bhv targ_charlie.bhv -f VNAME=$VNAME3      \
    START_POS=$START_POS3 LOITER_POS=$LOITER_POS3       

nsplug meta_vehicle.bhv targ_david.bhv -f VNAME=$VNAME4      \
    START_POS=$START_POS4 LOITER_POS=$LOITER_POS4       

nsplug meta_vehicle.bhv targ_ernie.bhv -f VNAME=$VNAME5      \
    START_POS=$START_POS5 LOITER_POS=$LOITER_POS5       

nsplug meta_uuv.bhv targ_prey.bhv -f VNAME=$VNAME9      \
    START_POS=$START_POS9 LOITER_POS=$LOITER_POS5      



if [ ${JUST_BUILD} = "yes" ] ; then
    exit 0
fi

#-------------------------------------------------------
#  Part 3: Launch the processes
#-------------------------------------------------------

printf "Launching $VNAME1 MOOS Community (WARP=%s) \n" $WARP
pAntler targ_archie.moos >& /dev/null &
sleep 1
printf "Launching $VNAME2 MOOS Community (WARP=%s) \n" $WARP
pAntler targ_betty.moos >& /dev/null &
sleep 1
printf "Launching $VNAME3 MOOS Community (WARP=%s) \n" $WARP
pAntler targ_charlie.moos >& /dev/null &
sleep 1
printf "Launching $VNAME4 MOOS Community (WARP=%s) \n" $WARP
pAntler targ_david.moos >& /dev/null &
sleep 1
printf "Launching $VNAME5 MOOS Community (WARP=%s) \n" $WARP
pAntler targ_ernie.moos >& /dev/null &
sleep 1
printf "Launching $VNAME9 MOOS Community (WARP=%s) \n" $WARP
pAntler targ_prey.moos >& /dev/null &
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


