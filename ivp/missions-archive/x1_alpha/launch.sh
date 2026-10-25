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

#----------------------------------------------------------
#  Part 2: Check for and handle command-line arguments

for ARGI; do
    if [ "${ARGI}" = "--help" -o "${ARGI}" = "-h" ]; then
	echo "launch.sh [OPTIONS] [time_warp]     "
	echo "                                    " 
	echo "Options:                            " 
	echo "  --help, -h                        " 
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
pAntler alpha.moos --MOOSTimeWarp=$TIME_WARP >& /dev/null &

uMAC  alpha.moos

echo; echo "Halting all apps"
kill -- -$$ 
