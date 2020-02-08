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

#-------------------------------------------------------
#  Part 1: Initialize global variables
#-------------------------------------------------------
HEADER="yes"
OS="osx"
RESFILE="$HOME/.ipaddrs"
DATE=$(date)

#-------------------------------------------------------
#  Part 2: Check for and handle command-line arguments
#-------------------------------------------------------
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
	echo "                                                        "
	echo "Examples:                                               "
 	echo "  $ ipaddrs.sh                                          "
 	echo "  $ ipaddrs.sh  --terse                                 "
        exit 0;
    elif [ "${ARGI}" = "--terse" -o "${ARGI}" = "-t" ] ; then
	HEADER="no"
    elif [ "${ARGI}" = "--info"   -o "${ARGI}" = "-i" ] ; then
	echo "List all active network interfaces and IP addresses "
	exit 0
    else
        echo "Bad Argument: "$ARGI
        exit 3
    fi
done


#-------------------------------------------------------
#  Part 3: Determine the operating system
#-------------------------------------------------------
if [ -d "/proc" ]; then
    OS="linux"
fi

#-------------------------------------------------------
#  Part 4: Ensure System Utilities are installed per OS
#-------------------------------------------------------
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


#-------------------------------------------------------
#  Part 5: Clear results file and Write file header if desired
#-------------------------------------------------------
echo -n "" > $RESFILE
if [ "$HEADER" = "yes" ]; then
    echo "// IP addresses of currently active network interfaces." >> $RESFILE
    echo "// Produced automatically by the ipaddrs.sh utility.   " >> $RESFILE
    echo "// $DATE" >> $RESFILE
fi

#-------------------------------------------------------
#  Part 6: Handle the MaxOS case
#-------------------------------------------------------
if [ "${OS}" = "osx" ]; then
    
    declare -a interfaces=()
    
    interfaces=( $(ifconfig | fgrep "flags=" | fgrep "mtu" | cut -d ' ' -f1) )

    count=0
    for iface in "${interfaces[@]}"
    do
	new_iface=${iface%?}
	ipaddr="$(ipconfig getifaddr $new_iface)"
	if [ $? = 0 ]; then
	    echo $ipaddr >> $RESFILE
	    ((count=count+1))
	fi
    done

    if [ ${count} > 1 ]; then
	exit 2
    elif [ ${count} = 0 ]; then
	exit 1
    fi
    
    exit 0

fi

#-------------------------------------------------------
#  Part 7: Handle the Linux case
#-------------------------------------------------------
declare -a interfaces=()

interfaces=( $(hostname -I) )

count=0
for iface in "${interfaces[@]}"
do
    echo $iface  >> $RESFILE
    ((count=count+1))
done

if [ ${count} > 1 ]; then
    exit 2
elif [ ${count} = 0 ]; then
    exit 1
fi

exit 0

