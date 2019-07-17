#!/bin/bash -e
#---------------------------------------------------------
# File: launch_shoreside.sh
# Name: Mike Benjamin
# Date: May 8th, 2019
# Note: Goal of general pavilion shoreside launch script
#---------------------------------------------------------
#  Part 1: Initialize configurable variables with defaults
#---------------------------------------------------------
TIME_WARP=1
JUST_MAKE="no"

SHORE_IPADDR="localhost"
SHORE_PSHARE="9200"
SHORE_MOOSDB="9000"

#---------------------------------------------------------
#  Part 2: Check for and handle command-line arguments
#---------------------------------------------------------
for ARGI; do
    if [ "${ARGI}" = "--help" -o "${ARGI}" = "-h" ] ; then
        echo "$0 [SWITCHES]"
        echo "  --shoreip=       Shore IP Address  (Default is $SHORE_IPADDR)"
        echo "  --shoreps=       Shore pShare port (Default is $SHORE_PSHARE)"
        echo "  --just_make, -j  Just build targ files, dont launch.     "
        echo "  --help, -h       Display this help message               "
        exit 0
    elif [ "${ARGI//[^0-9]/}" = "$ARGI" -a "$TIME_WARP" = 1 ]; then
        TIME_WARP=$ARGI
    elif [ "${ARGI}" = "--just_make" -o "${ARGI}" = "-j" ] ; then
        JUST_MAKE="yes"
    elif [ "${ARGI:0:11}" = "--shoreip=" ] ; then
        SHORE_IPADDR="${ARGI#--shoreip=*}"
    elif [ "${ARGI:0:13}" = "--shoreps=" ] ; then
        SHORE_PSHARE="${ARGI#--shoreps=*}"
    else
        echo "Bad arg:" $ARGI "Run with -h for help. Exiting (1)."
        exit 1
    fi
done

#---------------------------------------------------------
#  Part 3: Create the Shoreside MOOS file. 
#    Note: Failed nsplug will abort launch due to bash -e on line 1
#    Note: Undef macros will be aletered to user with nsplug -i flag
#---------------------------------------------------------
nsplug meta_shoreside.moos targ_shoreside.moos -i -f WARP=$TIME_WARP \
       SHORE_PSHARE=$SHORE_PSHARE  SHORE_MOOSDB=$SHORE_MOOSDB         \
       SHORE_IPADDR=$SHORE_IPADDR  

#---------------------------------------------------------
#  Part 4: Possibly exit now if we're just building targ files
#---------------------------------------------------------
if [ ${JUST_MAKE} = "yes" ] ; then
    echo "Shoreside targ files built. Nothing launched."
    exit 0
fi

#---------------------------------------------------------
#  Part 5: --Launch the Shoreside
#-------------------------------------------------------
echo "Launching Shoreside MOOS Community (WARP=$TIME_WARP)"
pAntler targ_shoreside.moos >& /dev/null &
echo "Done Launching Shoreside "

uMAC targ_shoreside.moos

echo "Killing all processes ... "
kill -- -$$
echo "Done killing processes.   "
