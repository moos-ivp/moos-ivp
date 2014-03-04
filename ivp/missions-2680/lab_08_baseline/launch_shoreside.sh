#!/bin/bash 

TIME_WARP=1
JUST_BUILD="no"

THISNAME=$(id -un)"@"$(hostname -s)
echo "Vehicle Name: " $THISNAME

#-------------------------------------------------------
#  Part 1: Check for and handle command-line arguments
#-------------------------------------------------------
for ARGI; do
    if [ "${ARGI}" = "--help" -o "${ARGI}" = "-h" ] ; then
	printf "%s [SWITCHES]         \n" $0
	printf "  --just_build, -j    \n" 
	printf "  --help, -h          \n" 
	exit 0;
    elif [ "${ARGI//[^0-9]/}" = "$ARGI" ]; then 
        TIME_WARP=$ARGI
    elif [ "${ARGI}" = "--just_build" -o "${ARGI}" = "-j" ] ; then
	JUST_BUILD="yes"
    else
	printf "Bad Argument: %s \n" $ARGI
	exit 0
    fi
done

#-------------------------------------------------------
#  Part 2: Create the .moos and .bhv files. 
#-------------------------------------------------------
SNAME="shoreside"
SPORT="9000"
SLPORT="9200"

nsplug meta_shoreside.moos targ_$SNAME.moos -f WARP=$TIME_WARP    \
    SHARE_LISTEN=$SLPORT  SPORT=$SPORT  SNAME=$SNAME

if [ ${JUST_BUILD} = "yes" ] ; then
    exit 0
fi

#-------------------------------------------------------
#  Part 3: Launch the processes
#-------------------------------------------------------
printf "Launching $SNAME MOOS Community (WARP=%s) \n" $TIME_WARP
pAntler targ_$SNAME.moos >& /dev/null &


#-------------------------------------------------------
#  Part 4: Exiting and/or killing the simulation
#-------------------------------------------------------
uMAC targ_shoreside.moos

# %1, matches the PID of the first job in the active jobs list
# namely the pAntler job(s) launched in Part 3.
printf "Killing all processes ... \n"
kill %1 
printf "Done killing processes.   \n"
