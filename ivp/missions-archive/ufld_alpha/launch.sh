#!/bin/bash -e
#--------------------------------------------------------------
#   Script: launch.sh
#  Mission: ufld_alpha
#   Author: Michael Benjamin  
#   LastEd: Sep 4th, 2022     
#-------------------------------------------------------------- 
#  Part 1: Define a convenience function for producing terminal
#          debugging/status output depending on the verbosity.
#-------------------------------------------------------------- 
vecho() { if [ "$VERBOSE" != "" ]; then echo "$ME: $1"; fi }

#--------------------------------------------------------------  
#  Part 2: Set global variables
#--------------------------------------------------------------
ME=`basename "$0"`
TIME_WARP=1
JUST_MAKE=""
VERBOSE=""
AUTO_LAUNCHED="no"
CMD_ARGS=""
NOCONFIRM="-nc"
PASS_ARGS=""

#--------------------------------------------------------------  
#  Part 3: Check for and handle command-line arguments
#--------------------------------------------------------------  
for ARGI; do
    CMD_ARGS+=" ${ARGI}"
    if [ "${ARGI}" = "--help" -o "${ARGI}" = "-h" ]; then
	echo "$ME [SWITCHES] [time_warp]         "
	echo "  --help, -h                                     " 
	echo "    Display this help message                    "
        echo "  --verbose, -v                                  "
        echo "    Increase verbosity                           "
	echo "  --just_make, -j                                " 
	echo "    Just make targ files, but do not launch      "
	echo "  --auto, -a                                     "
	echo "     Auto-launched by a script.                  "
	echo "     Will not launch uMAC as the final step.     "
	echo "  --pavlab, -p                                   "
	echo "    Set region to be MIT pavlab                  " 
	exit 0;
    elif [ "${ARGI//[^0-9]/}" = "$ARGI" -a "$TIME_WARP" = 1 ]; then 
        TIME_WARP=$ARGI
    elif [ "${ARGI}" = "--verbose" -o "${ARGI}" = "-v" ]; then
	VERBOSE="--verbose"
	NOCONFIRM=""
    elif [ "${ARGI}" = "--just_make" -o "${ARGI}" = "-j" ]; then
	JUST_MAKE="yes"
    elif [ "${ARGI}" = "--pavlab" -o "${ARGI}" = "-p" ]; then
        PASS_ARGS+=$ARGI
    else 
        echo "launch.sh Bad arg:" $ARGI " Exiting with code: 1"
        exit 1
    fi
done

#---------------------------------------------------------------
#  Part 4: If verbose, show vars and confirm before launching
#---------------------------------------------------------------
if [ "${VERBOSE}" != "" ]; then 
    echo "======================================================"
    echo "              launch.sh SUMMARY                       "
    echo "======================================================"
    echo "$ME"
    echo "CMD_ARGS  =  [${CMD_ARGS}]  "
    echo "VERBOSE   =  [${VERBOSE}]   "
    echo "JUST_MAKE =  [${JUST_MAKE}] "
    echo "TIME_WARP =  [${TIME_WARP}] "
    echo "PASS_ARGS =  [${PASS_ARGS}] "
    echo "--------------------------- "
    echo -n "Hit the RETURN key to continue with launching"
    read ANSWER
fi

PASS_ARGS+=" ${NOCONFIRM} ${JUST_MAKE} ${VERBOSE}"

#--------------------------------------------------------------  
#  Part 5: Actual launch
#--------------------------------------------------------------  
./launch_vehicle.sh   --auto $PASS_ARGS $TIME_WARP --vname=abe
sleep 1
./launch_shoreside.sh --auto $PASS_ARGS $TIME_WARP

#---------------------------------------------------------------
#  Part 6: If launched from script, we're done, exit now
#---------------------------------------------------------------
if [ "${AUTO_LAUNCHED}" = "yes" -o "${JUST_MAKE}" != "" ]; then
    exit 0
fi


#--------------------------------------------------------------  
#  Part 7: Launch uMAC until mission quit
#--------------------------------------------------------------  
uMAC --paused targ_shoreside.moos
kill -- -$$
