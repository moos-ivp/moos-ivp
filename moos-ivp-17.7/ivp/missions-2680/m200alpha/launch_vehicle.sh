#!/bin/bash 

JUST_BUILD="no"
SHOREIP="1.2.3.4"

#-------------------------------------------------------
#  Part 1: Check for and handle command-line arguments
#-------------------------------------------------------
for ARGI; do
    if [ "${ARGI}" = "--help" -o "${ARGI}" = "-h" ] ; then
	printf "%s [SWITCHES]                                \n" $0
	printf "  --evan, -e             evan vehicle only   \n"
	printf "  --felix, -f            felix vehicle only  \n"
	printf "  --just_build, -j                           \n" 
	printf "  --help, -h                                 \n" 
	exit 0;
    elif [ "${ARGI}" = "--felix" -o "${ARGI}" = "-f" ] ; then
        VNAME="felix"
    elif [ "${ARGI}" = "--evan" -o "${ARGI}" = "-e" ] ; then
        VNAME="evan"
    elif [ "${ARGI:0:10}" = "--shoreip=" ] ; then
        SHOREIP="${ARGI#--shoreip=*}"
    elif [ "${ARGI}" = "--just_build" -o "${ARGI}" = "-j" ] ; then
	JUST_BUILD="yes"
    else 
	printf "Bad Argument: %s \n" $ARGI
	exit 1
    fi
done

#-------------------------------------------------------
#  Part 2: Sanity Check the Command Line Args
#-------------------------------------------------------
if [ "${VNAME}" = "evan" ] ; then
    M200_IP="192.168.5.1"
elif [ "${VNAME}" = "felix" ] ; then
    M200_IP="192.168.6.1"
else 
    printf "Evan or Felix must be selected (-e or -f) \n"
    exit 1
fi

printf "vehicle " $VNAME " selected. \n"

#-------------------------------------------------------
#  Part 3: Create the .moos and .bhv files. 
#-------------------------------------------------------
nsplug meta_vehicle.moos targ_$VNAME.moos -f          \
    VNAME=$VNAME          SHARE_LISTEN=9300               \
    VPORT=9200            SHORE_LISTEN=$SHORE_LISTEN      \
    SHORE_IP=$SHOREIP     M200_IP=$M200_IP                \
    WARP=1                

nsplug meta_vehicle.bhv targ_$VNAME.bhv -f           \
    VNAME=$VNAME  SPEED=1                            \

if [ ${JUST_BUILD} = "yes" ] ; then
    printf "Files assembled; vehicle not launched; exiting per request.\n"
    exit 0
fi

#-------------------------------------------------------
#  Part 4: Launch the processes
#-------------------------------------------------------
printf "Launching $VNAME MOOS Community \n"
pAntler targ_$VNAME.moos >& /dev/null &
uMAC targ_$VNAME.moos

printf "Killing all processes ... \n "
kill %1 
