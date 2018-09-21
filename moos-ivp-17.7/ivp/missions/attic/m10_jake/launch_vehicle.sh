#!/bin/bash 
#-------------------------------------------------------
#  Part 1: Check for and handle command-line arguments
#-------------------------------------------------------
TIME_WARP=1
VNAME="archie"
START_POS="0,0"
JUST_MAKE="no"
INDEX="01"
for ARGI; do
    if [ "${ARGI}" = "--help" -o "${ARGI}" = "-h" ] ; then
	printf "%s [SWITCHES] [time_warp]   \n" $0
	printf "  --just_make, -j    \n" 
	printf "  --help, -h         \n" 
	printf "  --vname=VNAME      \n" 
	printf "  --startpos=X,Y     \n" 
	printf "  --index=INDEX      \n" 
	exit 0;
    elif [ "${ARGI//[^0-9]/}" = "$ARGI" -a "$TIME_WARP" = 1 ]; then 
        TIME_WARP=$ARGI
    elif [ "${ARGI:0:8}" = "--vname=" ] ; then
        VNAME="${ARGI#--vname=*}"
    elif [ "${ARGI:0:8}" = "--index=" ] ; then
        INDEX="${ARGI#--index=*}"
    elif [ "${ARGI:0:11}" = "--startpos=" ] ; then
        START_POS="${ARGI#--startpos=*}"
    elif [ "${ARGI}" = "--just_build" -o "${ARGI}" = "-j" ] ; then
	JUST_MAKE="yes"
    else 
	printf "Bad Argument: %s \n" $ARGI
	exit 0
    fi
done

#-------------------------------------------------------
#  Part 2: Create the .moos and .bhv files. 
#-------------------------------------------------------
nsplug meta_$VNAME.moos targ_$VNAME.moos -f \
   WARP=$TIME_WARP         \
   VNAME=$VNAME            \
   VPORT="90"$INDEX        \
   VTYPE=UUV               \
   START_POS=$START_POS    \
   SHARE_LISTEN="93"$INDEX  

nsplug meta_$VNAME.bhv targ_$VNAME.bhv -f \
    VNAME=$VNAME                          \
    START_POS=$START_POS 

if [ ${JUST_MAKE} = "yes" ] ; then
    exit 0
fi

#-------------------------------------------------------
#  Part 3: Launch the processes
#-------------------------------------------------------
printf "Launching $VNAME MOOS Community (WARP=%s) \n" $TIME_WARP
pAntler targ_$VNAME.moos >& /dev/null &

uMAC targ_$VNAME.moos

printf "Killing all processes ... \n"
kill %1 
printf "Done killing processes.   \n"


