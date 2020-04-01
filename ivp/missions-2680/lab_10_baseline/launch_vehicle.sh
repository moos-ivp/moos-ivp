#!/bin/bash 
#-------------------------------------------------------
#  Part 1: Check for and handle command-line arguments
#-------------------------------------------------------
TIME_WARP=1
JUST_MAKE="no"
HOSTNAME=$(hostname -s)
VNAME=$(id -un)

MOOS_PORT="9001"
UDP_LISTEN_PORT="9201"
SHOREIP="localhost"
SHORE_LISTEN="9200"

#-------------------------------------------------------
#  Part 2: Check for and handle command-line arguments
#-------------------------------------------------------
for ARGI; do
    if [ "${ARGI}" = "--help" -o "${ARGI}" = "-h" ] ; then
	echo "%s [SWITCHES]                            "
	echo "  --vname=VEHICLE_NAME                   " 
	echo "  --shore=IP address of shoreside        " 
	echo "  --mport=MOOSDB Port #                  " 
	echo "  --lport=pShare UDPListen Port #        " 
	echo "  --just_make, -j                        " 
	echo "  --help, -h                             " 
	exit 0;
    elif [ "${ARGI//[^0-9]/}" = "$ARGI" -a "$TIME_WARP" = 1 ]; then 
        TIME_WARP=$ARGI
    elif [ "${ARGI:0:8}" = "--shore=" ] ; then
	SHOREIP="${ARGI#--shore=*}"
    elif [ "${ARGI:0:7}" = "--mport" ] ; then
	MOOS_PORT="${ARGI#--mport=*}"
    elif [ "${ARGI:0:7}" = "--lport" ] ; then
	UDP_LISTEN_PORT="${ARGI#--lport=*}"
    elif [ "${ARGI:0:7}" = "--vname" ] ; then
	VNAME="${ARGI#--vname=*}"
    elif [ "${ARGI}" = "--just_make" -o "${ARGI}" = "-j" ] ; then
	JUST_MADE="yes"
    else
	echo "launch_vehicle.sh: Bad Arg: " $ARGI
	exit 0
    fi
done


#-------------------------------------------------------
#  Part 3: Create the .moos and .bhv files. 
#-------------------------------------------------------

FULL_VNAME=$VNAME"@"$HOSTNAME
WPT_COLOR="light_blue"

# Generate a random start position in range x=[0,180], y=[0,-50]
X_START_POS=$(($RANDOM % 180))
Y_START_POS=$((($RANDOM % 50)  - 50))
# Generate a random start position in range x=[0,180], y=[0,-125]
X_LOITER_POS=$(($RANDOM % 180))
Y_LOITER_POS=$((($RANDOM % 50) - 125))
START_POS="$X_START_POS,$Y_START_POS" 
LOITER_POS="x=$X_LOITER_POS,y=$Y_LOITER_POS" 

nsplug meta_vehicle.moos targ_$FULL_VNAME.moos -f WARP=$TIME_WARP       \
    VNAME=$FULL_VNAME  VPORT=$MOOS_PORT  SHARE_LISTEN=$UDP_LISTEN_PORT  \
    START_POS=$START_POS SHOREIP=$SHOREIP SHORE_LISTEN=$SHORE_LISTEN

nsplug meta_vehicle.bhv targ_$FULL_VNAME.bhv -f VNAME=$FULL_VNAME       \
    START_POS=$START_POS LOITER_POS=$LOITER_POS       
   
 
if [ ${JUST_MAKE} = "yes" ] ; then
    exit 0
fi

#-------------------------------------------------------
#  Part 3: Launch the processes
#-------------------------------------------------------
printf "Launching $VNAME MOOS Community (WARP=%s) \n" $TIME_WARP
pAntler targ_$FULL_VNAME.moos >& /dev/null &

#-------------------------------------------------------
#  Part 4: Exiting and/or killing the simulation
#-------------------------------------------------------

uMAC targ_$FULL_VNAME.moos

# %1, matches the PID of the first job in the active jobs list
# namely the pAntler job(s) launched in Part 3.
printf "Killing all processes ... \n"
kill %1 
printf "Done killing processes.   \n"
