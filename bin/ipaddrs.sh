#!/bin/bash 
#--------------------------------------------------------------
#  Script: ippaddrs.sh
#  Author: Michael Benjamin
#    Date: Feb 8th 2020
#   About: This script will get a list of the IP addresses for 
#          all enabled network interfaces. It will auto-detect
#          if it is being invoked on a linux or MacOS machine
#          and perform the relevent linux or MacOS system calls.
#    Uses: OSX: ipconfig, ifconfig - (getting network interface info)
#          Linux: hostname - (getting network interface info)
#--------------------------------------------------------------
#  Part 1: Define a convenience function for producing terminal
#          debugging/status output depending on the verbosity.
#-------------------------------------------------------------- 
vecho()  { if [ "$VERBOSE" != "" ]; then echo $1; fi }
vechon() { if [ "$VERBOSE" != "" ]; then echo -n $1; fi }

#--------------------------------------------------------------
#  Part 2: Initialize global variables
#--------------------------------------------------------------
VERBOSE=""
HEADER="yes"
BLUNT="no"
OS="osx"
MATCH=""
MATCH_RES=""
TMP_RESFILE="$HOME/.ipaddrs_pid$$"
RESFILE="$HOME/.ipaddrs"
BLUNT_IP="no"


#--------------------------------------------------------------
#  Part 3: Check for and handle command-line arguments
#--------------------------------------------------------------
for ARGI; do
    if [ "${ARGI}" = "--help" -o "${ARGI}" = "-h" ] ; then
        echo "ipaddrs.sh [OPTIONS]                                    "
	echo "                                                        "
	echo "SYNOPSIS:                                               "
	echo "  This script will get a list of all enabled network    "
        echo "  interfaces and their IP addresses. It will auto-detect"
        echo "  if it is being invoked on a linux or MacOS machine    "
	echo "  and perform the relevent linux or MacOS system calls. "
	echo "                                                        "
        echo "Options:                                                "
        echo "  --help,    -h      Display this help message          " 
        echo "  --info,    -i      Display short script description   " 
        echo "  --terse,   -t      Suppress header info               " 
        echo "  --blunt,   -b      Single line only to terminal, no CR" 
        echo "  --verbose, -v      Verbose output IPs to terminal     " 
        echo "  --match=<pattern>  Return an IP matching a pattern    " 
	echo "                                                        "
	echo "Returns:                                                "
	echo "  0 on --help, -h or --info, -i                         "
	echo "  0 If a single IP address and interface found          "
	echo "  1 If no interfaces have been found                    "
	echo "  2 If multiple interfaces have been found              "
	echo "  3 Upon bad argument                                   "
	echo "  4 Missing the hostname system command                 "
	echo "  5 Missing the ipconfig system command                 "
	echo "  6 Missing the ifconfig system command                 "
	echo "  7 No IP address or interface found                    "
	echo "                                                        "
	echo "Examples:                                               "
 	echo "  $ ipaddrs.sh                                          "
 	echo "  $ ipaddrs.sh  --terse                                 "
 	echo "  $ ipaddrs.sh  --match=192.168.7                       "
        exit 0;
    elif [ "${ARGI}" = "--verbose" -o "${ARGI}" = "-v" ]; then
	VERBOSE="yes"
    elif [ "${ARGI}" = "--terse" -o "${ARGI}" = "-t" ]; then
	HEADER="no"
    elif [ "${ARGI}" = "--blunt" -o "${ARGI}" = "-b" ]; then
	BLUNT="yes"
    elif [ "${ARGI:0:8}" = "--match=" ]; then
        MATCH="${ARGI#--match=*}"
    elif [ "${ARGI}" = "--info"   -o "${ARGI}" = "-i" ]; then
	echo "List all active network interfaces and IP addresses "
	exit 0
    else
        echo "ipaddrs.sh: Bad Arg: $ARGI. Exit Code 3"
        exit 3
    fi
done


#--------------------------------------------------------------
#  Part 4: Determine the operating system
#--------------------------------------------------------------
if [ -d "/proc" ]; then
    OS="linux"
fi

#--------------------------------------------------------------
#  Part 5: Ensure System Utilities are installed per OS
#--------------------------------------------------------------
if [ "${OS}" = "linux" ]; then
    # Make sure the hostname utility is present
    command -v hostname >& /dev/null
    if [ $? != 0 ]; then
	echo "The required utility hostname is not found. Exiting."
	exit 4
    fi
fi

if [ "${OS}" = "osx" ]; then
    # Make sure the ipconfig utility is present
    command -v ipconfig >& /dev/null
    if [ $? != 0 ]; then
	echo "The required utility ipconfig is not found. Exiting."
	exit 5
    fi

    # Make sure the ifconfig utility is present
    command -v ifconfig >& /dev/null
    if [ $? != 0 ]; then
	echo "The required utility ifconfig is not found. Exiting."
	exit 6
    fi
fi

# Unless changed, default exit code indicates no ipaddr found
EXIT_CODE=7

#--------------------------------------------------------------
#  Part 6: Clear results file and Write file header if desired
#--------------------------------------------------------------
echo -n "" > $TMP_RESFILE
if [ "$HEADER" = "yes" -a "$BLUNT_IP" = "no" ]; then
    DATE=`date`
    echo "// IP addresses of currently active network interfaces." >> $TMP_RESFILE
    echo "// Produced by ipaddrs.sh utility: $DATE   " >> $TMP_RESFILE
fi

#--------------------------------------------------------------
#  Part 7: Handle the MaxOS case
#--------------------------------------------------------------
if [ "${OS}" = "osx" ]; then
    
    addresses=( $(ifconfig | fgrep "ffff" | cut -d ' ' -f2) )

    count=0
    for addr in "${addresses[@]}"
    do
	if [ "${MATCH}" != "" ]; then
	    if [[ "$addr" != "$MATCH"* ]]; then
		continue
	    fi
	fi
	
	BLUNT_IP=$addr
	vecho "addr:$addr"
	echo $addr >> $TMP_RESFILE
	((count=count+1))
    done

    if [ ${count} -gt 1 ]; then
	EXIT_CODE=2
    elif [ ${count} = 0 ]; then
	EXIT_CODE=1
    else
	EXIT_CODE=0
    fi
fi

#--------------------------------------------------------------
#  Part 8: Handle the Linux case
#--------------------------------------------------------------
if [ "${OS}" = "linux" ]; then

    declare -a interfaces=()
    
    interfaces=( $(hostname -I) )
    
    count=0
    for iface in "${interfaces[@]}"
    do
	if [ "${MATCH}" != "" ]; then
	    if [[ "$addr" != "$MATCH"* ]]; then
		continue
	    fi
	fi

	BLUNT_IP=$iface
	vecho "addr:$iface"
	echo $iface  >> $TMP_RESFILE
	((count=count+1))
    done
    
    if [ ${count} -gt 1 ]; then
	EXIT_CODE=2
    elif [ ${count} = 0 ]; then
	EXIT_CODE=1
    else
	EXIT_CODE=0	    
    fi
fi

#--------------------------------------------------------------
#  Part 9: If blunt_ip requested, just output the single IP to
#          the terminal and remove the temp file
#--------------------------------------------------------------
if [ "$BLUNT" = "yes" ]; then
    if [ $EXIT_CODE = 0 ]; then
	echo -n $BLUNT_IP
	rm -f $TMP_RESFILE
	exit 0
    fi
fi
    
#--------------------------------------------------------------
#  Part 10: If the tmp file is different, install it.  By building a
#  tmp file, with a unique file name based on the PID, this gaurds
#  against possibly several versions of this script running at nearly
#  the same time, and possiblly all writing to the destinatin file.
#--------------------------------------------------------------

diff $TMP_RESFILE $RESFILE >& /dev/null

if [ $? != 0 ]; then
    mv -f $TMP_RESFILE $RESFILE
else
    rm -f $TMP_RESFILE
fi

exit $EXIT_CODE

