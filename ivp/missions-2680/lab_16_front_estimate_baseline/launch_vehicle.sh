#!/bin/bash -e
#--------------------------------------------------------------  
#  Part 1: Declare global var defaults
#--------------------------------------------------------------
TIME_WARP=1
JUST_MAKE="no"
VNAME="archie"
COOL_FAC=50
COOL_STEPS=1000
CONCURRENT="true"
ADAPTIVE="false"

START_POS="0,0"
SURVEY_X=70
SURVEY_Y=-100
HEIGHT1=150
WIDTH1=120
LANE_WIDTH1=25
DEGREES1=270

IP_ADDR="localhost"
PSHARE_PORT="9301"
SHORE="localhost:9300"

#-------------------------------------------------------
#  Part 2: Check for and handle command-line arguments
#-------------------------------------------------------
for ARGI; do
    if [ "${ARGI}" = "--help" -o "${ARGI}" = "-h" ] ; then
	echo "launch_vehicle.sh [SWITCHES] [time_warp]   "
	echo "  --just_make, -j                          " 
	echo "  --vname=VNAME                            " 
	echo "  --help, -h                               "
	echo "  --ip=<addr>       (default is localhost) " 
	echo "  --shore=<addr>    (default is localhost:9300) " 
	echo "  --warp=WARP_VALUE                        "
	echo "  --adaptive, -a                           "
	echo "  --unconcurrent, -uc                      "
	echo "  --angle=DEGREE_VALUE                     "
	echo "  --cool=COOL_FAC                          "
	exit 0;
    elif [ "${ARGI:0:8}" = "--vname=" ]; then
        VNAME="${ARGI#--vname=*}"
    elif [ "${ARGI//[^0-9]/}" = "$ARGI" -a "$TIME_WARP" = 1 ]; then 
        TIME_WARP=$ARGI
    elif [ "${ARGI}" = "--just_make" -o "${ARGI}" = "-j" ]; then
	JUST_MAKE="yes"
    elif [ "${ARGI:0:6}" = "--warp" ]; then
        WARP="${ARGI#--warp=*}"
        UNDEFINED_ARG=""
    elif [ "${ARGI:0:6}" = "--cool" ]; then
        COOL_FAC="${ARGI#--cool=*}"
        UNDEFINED_ARG=""
    elif [ "${ARGI:0:7}" = "--angle" ]; then
        DEGREES1="${ARGI#--angle=*}"
        UNDEFINED_ARG=""
    elif [ "${ARGI}" = "--unconcurrent" -o "${ARGI}" = "-uc" ]; then
        CONCURRENT="false"
        UNDEFINED_ARG=""
    elif [ "${ARGI}" = "--adaptive" -o "${ARGI}" = "-a" ]; then
        ADAPTIVE="true"
        UNDEFINED_ARG=""
    elif [ "${ARGI:0:5}" = "--ip=" ]; then
        IP_ADDR="${ARGI#--ip=*}"
    elif [ "${ARGI:0:8}" = "--shore=" ] ; then
        SHORE="${ARGI#--shore=*}"
    elif [ "${ARGI:0:9}" = "--pshare=" ]; then
        PSHARE_PORT="${ARGI#--pshare=*}"
    else 
	echo "launch_vehicle.sh: Bad Arg:" $ARGI
	exit 0
    fi
done

#-------------------------------------------------------
#  Part 3: Create the .moos and .bhv files. 
#-------------------------------------------------------
nsplug meta_vehicle.moos targ_$VNAME.moos -f WARP=$TIME_WARP  \
   VNAME=$VNAME      START_POS=$START_POS                    \
   VPORT="9001"      PSHARE_PORT="9301"                      \
   SHORE=$SHORE      IP_ADDR=$IP_ADDR                        \
   VTYPE=UUV          COOL_FAC=$COOL_FAC  COOL_STEPS=$COOL_STEPS\
   CONCURRENT=$CONCURRENT  ADAPTIVE=$ADAPTIVE

nsplug meta_vehicle.bhv targ_$VNAME.bhv -f VNAME=$VNAME      \
    START_POS=$START_POS SURVEY_X=$SURVEY_X SURVEY_Y=$SURVEY_Y \
        HEIGHT=$HEIGHT1   WIDTH=$WIDTH1 LANE_WIDTH=$LANE_WIDTH1 \
        DEGREES=$DEGREES1

if [ ${JUST_MAKE} = "yes" ] ; then
    exit 0
fi

#-------------------------------------------------------
#  Part 4: Launch the processes
#-------------------------------------------------------
echo "Launching $VNAME MOOS Community with WARP:" $TIME_WARP
pAntler targ_$VNAME.moos >& /dev/null &

uMAC targ_$VNAME.moos

echo "Killing all processes ... "
kill -- -$$
echo "Done killing processes.   "


