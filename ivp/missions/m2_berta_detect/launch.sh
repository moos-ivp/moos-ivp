#!/bin/bash -e
#-------------------------------------------------------
#  Part 1: Check for and handle command-line arguments
#-------------------------------------------------------
TIME_WARP=1
JUST_MAKE="no"
LAUNCH_GUI="yes"
for ARGI; do
    if [ "${ARGI}" = "--help" -o "${ARGI}" = "-h" ] ; then
	printf "%s [SWITCHES] [time_warp]   \n" $0
	printf "  --just_make, -j    \n" 
	printf "  --help, -h         \n" 
	printf "  --no_gui, -n       \n" 
	exit 0;
    elif [ "${ARGI//[^0-9]/}" = "$ARGI" -a "$TIME_WARP" = 1 ]; then 
        TIME_WARP=$ARGI
    elif [ "${ARGI}" = "--just_build" -o "${ARGI}" = "-j" ] ; then
	JUST_MAKE="yes"
    elif [ "${ARGI}" = "--no_gui" -o "${ARGI}" = "-n" ] ; then
	LAUNCH_GUI="no"
    else 
	printf "Bad Argument: %s \n" $ARGI
	exit 0
    fi
done

#-------------------------------------------------------
#  Part 2: Create the .moos and .bhv files. 
#-------------------------------------------------------
VNAME1="abe"           # The first vehicle Community
VNAME2="ben"           # The second vehicle Community
VNAME3="cal"           # The third vehicle Community
VNAME4="deb"           # The fourth vehicle Community
START_POS1="0,0"         
START_POS2="80,0"        
START_POS3="40,20"        
START_POS4="60,20"        
LOITER_POS1="x=0,y=-75"
LOITER_POS2="x=125,y=-50"
SHORE_LISTEN="9300"

nsplug meta_vehicle.moos targ_abe.moos -f WARP=$TIME_WARP \
    VNAME=$VNAME1          SHARE_LISTEN="9301"            \
    VPORT="9001"           SHORE_LISTEN=$SHORE_LISTEN     \
    START_POS=$START_POS1  

nsplug meta_vehicle.moos targ_ben.moos -f WARP=$TIME_WARP \
    VNAME=$VNAME2          SHARE_LISTEN="9302"            \
    VPORT="9002"           SHORE_LISTEN=$SHORE_LISTEN     \
    START_POS=$START_POS2  

nsplug meta_vehicle.moos targ_cal.moos -f WARP=$TIME_WARP \
    VNAME=$VNAME3          SHARE_LISTEN="9303"            \
    VPORT="9003"           SHORE_LISTEN=$SHORE_LISTEN     \
    START_POS=$START_POS3  

nsplug meta_vehicle.moos targ_deb.moos -f WARP=$TIME_WARP \
    VNAME=$VNAME4          SHARE_LISTEN="9304"            \
    VPORT="9004"           SHORE_LISTEN=$SHORE_LISTEN     \
    START_POS=$START_POS4  

nsplug meta_shoreside.moos targ_shoreside.moos -f WARP=$TIME_WARP \
    SNAME="shoreside"  SHARE_LISTEN=$SHORE_LISTEN                 \
    SPORT="9000"       VNAME1=$VNAME1                             \
    VNAME2=$VNAME2     LAUNCH_GUI=$LAUNCH_GUI

nsplug meta_vehicle.bhv targ_abe.bhv -f VNAME=$VNAME1     \
    START_POS=$START_POS1 LOITER_POS=$LOITER_POS1 MISSION_ROLE="SIDE"      

nsplug meta_vehicle.bhv targ_ben.bhv -f VNAME=$VNAME2     \
    START_POS=$START_POS2 LOITER_POS=$LOITER_POS2 MISSION_ROLE="SIDE"      

nsplug meta_vehicle.bhv targ_cal.bhv -f VNAME=$VNAME3     \
    START_POS=$START_POS3 MISSION_ROLE="MIDDLE"

nsplug meta_vehicle.bhv targ_deb.bhv -f VNAME=$VNAME4     \
    START_POS=$START_POS4 MISSION_ROLE="MIDDLE"

if [ ! -e targ_abe.moos ]; then echo "no targ_abe.moos"; exit; fi
if [ ! -e targ_abe.bhv  ]; then echo "no targ_abe.bhv "; exit; fi
if [ ! -e targ_ben.moos ]; then echo "no targ_ben.moos"; exit; fi
if [ ! -e targ_ben.bhv  ]; then echo "no targ_ben.bhv "; exit; fi
if [ ! -e targ_cal.moos ]; then echo "no targ_cal.moos"; exit; fi
if [ ! -e targ_cal.bhv  ]; then echo "no targ_cal.bhv "; exit; fi
if [ ! -e targ_deb.moos ]; then echo "no targ_deb.moos"; exit; fi
if [ ! -e targ_deb.bhv  ]; then echo "no targ_deb.bhv "; exit; fi
if [ ! -e targ_shoreside.moos ]; then echo "no targ_shoreside.moos";  exit; fi

if [ ${JUST_MAKE} = "yes" ] ; then
    exit 0
fi


#-------------------------------------------------------
#  Part 3: Launch the processes
#-------------------------------------------------------
printf "Launching $SNAME MOOS Community (WARP=%s) \n"  $TIME_WARP
pAntler targ_shoreside.moos >& /dev/null &
printf "Launching $VNAME1 MOOS Community (WARP=%s) \n" $TIME_WARP
pAntler targ_abe.moos >& /dev/null &
printf "Launching $VNAME2 MOOS Community (WARP=%s) \n" $TIME_WARP
pAntler targ_ben.moos >& /dev/null &
printf "Launching $VNAME3 MOOS Community (WARP=%s) \n" $TIME_WARP
pAntler targ_cal.moos >& /dev/null &
printf "Launching $VNAME4 MOOS Community (WARP=%s) \n" $TIME_WARP
pAntler targ_deb.moos >& /dev/null &
printf "Done \n"

uMAC targ_shoreside.moos

printf "Killing all processes ... \n"
mykill; ktm
printf "Done killing processes.   \n"
