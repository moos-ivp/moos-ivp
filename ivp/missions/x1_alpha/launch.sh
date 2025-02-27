  #!/bin/bash 
#----------------------------------------------------------
#  Script: launch.sh
#  Author: Michael Benjamin
#  LastEd: May 20th 2019
#----------------------------------------------------------
#  Part 1: Set convenience functions for producing terminal
#          debugging output, and catching SIGINT (ctrl-c).
#----------------------------------------------------------
vecho() { if [ "$VERBOSE" != "" ]; then echo "$ME: $1"; fi }
on_exit() { echo; echo "Halting all apps"; kill -- -$$; }
trap on_exit SIGINT

TIME_WARP=1
COMMUNITY="alpha"

#----------------------------------------------------------
#  Part 1: Check for and handle command-line arguments
TIME_WARP=1
for ARGI; do
    if [ "${ARGI}" = "--help" -o "${ARGI}" = "-h" ]; then
	echo "launch.sh [SWITCHES] [time_warp]     "
	echo "  --help, -h                         " 
	exit 0;
    elif [ "${ARGI//[^0-9]/}" = "$ARGI" -a "$TIME_WARP" = 1 ]; then 
        TIME_WARP=$ARGI
    else 
        echo "Launch.sh Bad arg:" $ARGI " Exiting with code: 1"
	exit 1
    fi
done

#----------------------------------------------------------
#  Part 3: Launch the processes
#----------------------------------------------------------
echo "Launching $COMMUNITY MOOS Community. WARP is" $TIME_WARP
pAntler $COMMUNITY.moos --MOOSTimeWarp=$TIME_WARP >& /dev/null &

uMAC  $COMMUNITY.moos

echo; echo "Halting all apps"
kill -- -$$ 
