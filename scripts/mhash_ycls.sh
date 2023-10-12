#!/bin/bash 
#-------------------------------------------------------------- 
#   Script: mhash_ycls.sh     
#   Author: Michael Benjamin   
#   LastEd: July 2023
#-------------------------------------------------------------- 
#  Part 1: Define a convenience function for producing terminal
#          debugging/status output depending on the verbosity.
#-------------------------------------------------------------- 
vecho() { if [ "$VERBOSE" != "" ]; then echo "$ME: $1"; fi }

#-------------------------------------------------------------- 
#  Part 2: Set Global variables
#-------------------------------------------------------------- 
ME=`basename "$0"`
TIME_WARP=1
JUST_MAKE="no"
IP="oceanai.mit.edu"

#-------------------------------------------------------
#  Part 3: Check for and handle command-line arguments
#-------------------------------------------------------
for ARGI; do
    if [ "${ARGI}" = "--help" -o "${ARGI}" = "-h" ]; then
	echo "$ME [OPTIONS] [time_warp]                               "
	echo "                                                        "
        echo "Synopsis:                                               "
	echo "  A script for launching an arbitrary number of vehicles"
	echo "  either all in simulation on this machine, or with the "
	echo "  --mtasc flag, launched across multiple machines in the"
	echo "  same subnet.                                          "
	echo "                                                        "
	echo "Options:                                                "
        echo "  --help, -h                                            "
        echo "    Display this help message                           "
        echo "  --verbose, -v                                         "
        echo "    Increase verbosity                                  "
	echo "  --just_make, -j                                       " 
	echo "    Just make the targ files, but do not launch.        " 
	echo "                                                        "
	echo "  --mtasc, -m                                           " 
	echo "    Launch vehicles in the MTASC cluster                " 
	echo "  --shoreip=<ipaddr>                                    " 
	echo "    IP address where nodes can expect to find shoreside "
	echo "  --logclean, -l                                        " 
        echo "    Clean (remove) all log files prior to launch        "
	echo "                                                        "
	exit 0;
    elif [ "${ARGI}" = "--verbose" -o "${ARGI}" = "-v" ]; then
	VERBOSE="--verbose"
    elif [ "${ARGI//[^0-9]/}" = "$ARGI" -a "$TIME_WARP" = 1 ]; then 
        TIME_WARP=$ARGI
    elif [ "${ARGI}" = "--mtasc" -o "${ARGI}" = "-m" ]; then
	MTASC="yes"
    elif [ "${ARGI:0:10}" = "--shoreip=" ]; then
        SHOREIP="${ARGI#--shoreip=*}"
    else 
	echo "$ME: Bad Arg: $ARGI. Exit Code 1."
	exit 1
    fi
done

SSH_OPTIONS="-o StrictHostKeyChecking=no -o LogLevel=QUIET "
SSH_OPTIONS+="-o ConnectTimeout=1 -o ConnectionAttempts=1 "
SSH_OPTIONS+="-o BatchMode=yes "

ssh $IP $SSH_OPTIONS ls /home/yodacora &

"ssh -o StrictHostKeyChecking=no -o LogLevel=QUIET -o ConnectTimeout=1 -o ConnectionAttempts=1 -o BatchMode=yes $IP ls /home/yodacora"




