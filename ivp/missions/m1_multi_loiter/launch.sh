#!/bin/sh 

WARP=1
HELP="no"
BAD_ARGS=""

for ARGI; do
    UNDEFINED_ARG=$ARGI
    if [ "${ARGI:0:6}" = "--warp" ] ; then
	WARP="${ARGI#--warp=*}"
	UNDEFINED_ARG=""
    fi
    if [ "${ARGI}" = "--help" ] ; then
	HELP="yes"
	UNDEFINED_ARG=""
    fi
    if [ "${ARGI}" = "-h" ] ; then
	HELP="yes"
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
    printf "  --help, -h          \n" 
    exit 127;
fi

# Second check that the one argument is numerical
if [ "${WARP//[^0-9]/}" != "$WARP" ]; then 
    printf "Warp values must be numerical. Exiting now."
    exit 127
fi

#-------------------------------------------------------
#  Part 2: Create the .moos and .bhv files. 
#-------------------------------------------------------

VNAME1="vehicle1"  # Vehicle 1 Community
VPORT1="9201"
VNAME2="vehicle2"  # Vehicle 2 Community
VPORT2="9202"
SNAME="shoreside"  # Shoreside Community
SPORT="9200"

nsplug meta_vehicle.moos vehicle1.moos -f WARP=$WARP VNAME=$VNAME1 \
   VNAME1=$VNAME1 VNAME2=$VNAME2 VPORT1=$VPORT1 VPORT2=$VPORT2     \
   VPORT=$VPORT1 SPORT=$SPORT SNAME=$SNAME

nsplug meta_vehicle.moos vehicle2.moos -f WARP=$WARP VNAME=$VNAME2 \
   VNAME1=$VNAME1 VNAME2=$VNAME2 VPORT1=$VPORT1 VPORT2=$VPORT2     \
   VPORT=$VPORT2 SPORT=$SPORT SNAME=$SNAME

nsplug meta_shoreside.moos shoreside.moos -f WARP=$WARP         \
   VNAME1=$VNAME1 VNAME2=$VNAME2 VPORT1=$VPORT1 VPORT2=$VPORT2  \
   SPORT=$SPORT SNAME=$SNAME

#-------------------------------------------------------
#  Part 3: Launch the processes
#-------------------------------------------------------

printf "Launching $VNAME1 MOOS Community (WARP=$WARP) \n"
pAntler vehicle1.moos  >& /dev/null &
sleep 1
printf "Launching $VNAME2 MOOS Community (WARP=$WARP) \n"
pAntler vehicle2.moos  >& /dev/null &
sleep 1
printf "Launching $SNAME MOOS Community (WARP=$WARP) \n"
pAntler shoreside.moos >& /dev/null &
printf "Done \n"

