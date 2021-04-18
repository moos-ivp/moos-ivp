#!/bin/bash 
#--------------------------------------------------------------
#  Script: ippaddr_info.sh
#  Author: Michael Benjamin
#    Date: Feb 9th 2020
#   About: This script will check a list of well-known interface 
#          names and check if there is currently an IP address   
#          associated with each. The script is meant to be an    
#          FYI utility, not in a critical path. It may require   
#          regular updates as interface naming conventions       
#          seem to evolve/change every few years or so.          
#--------------------------------------------------------------

#-------------------------------------------------------
#  Part 1: Initialize global variables
#-------------------------------------------------------
HEADER="yes"
OS="osx"
TMP_RESFILE="$HOME/.ipaddr_info_pid$$"
ERRFILE="$HOME/.ipaddr_info_err"
RESFILE="$HOME/.ipaddr_info"

#-------------------------------------------------------
#  Part 2: Check for and handle command-line arguments
#-------------------------------------------------------
for ARGI; do
    if [ "${ARGI}" = "--help" -o "${ARGI}" = "-h" ] ; then
        echo "ipaddr_info.sh [OPTIONS]                                "
	echo "                                                        "
	echo "SYNOPSIS:                                               "
	echo "  This script will check a list of well-known interface "
        echo "  names and check if there is currently an IP address   "
        echo "  associated with each. The script is meant to be an    "
	echo "  FYI utility, not in a critical path. It may require   "
	echo "  regular updates as interface naming conventions       "
	echo "  seem to evolve/change every few years or so.          "
	echo "                                                        "
        echo "Options:                                                "
        echo "  --help,    -h      Display this help message          " 
        echo "  --info,    -i      Display short script description   " 
        echo "  --terse,   -t      Suppress header info               " 
	echo "                                                        "
	echo "Returns:                                                "
	echo "  0 on --help, -h or --info, -i                         "
	echo "  0 Normal error free run                               "
	echo "  1 Upon bad argument                                   "
	echo "  2 Missing the ifconfig linux system command           "
	echo "  3 Missing the networksetup MacOS system command       "
	echo "                                                        "
	echo "Examples:                                               "
 	echo "  $ ipaddr_info.sh                                      "
 	echo "  $ ipaddr_info.sh  --terse                             "
        exit 0;
    elif [ "${ARGI}" = "--terse" -o "${ARGI}" = "-t" ] ; then
	HEADER="no"
    elif [ "${ARGI}" = "--info"   -o "${ARGI}" = "-i" ] ; then
	echo "List a few common network interfaces and IP addresses  "
	exit 0
    else
        echo "Bad Argument: "$ARGI
        exit 3
    fi
done

#-------------------------------------------------------
#  Part 3: Define function for testing valid IP address
#-------------------------------------------------------
ipvalid() {
    test_addr=$1

    # Test 1: Must be exactly three decimal points
    char="."
    dots=`echo "${test_addr}" | awk -F"${char}" '{print NF-1}'`

    if [[ $dots -ne 3 ]]; then
	return 1
    fi
    
    field1=`echo $test_addr |  cut -d '.' -f1` 
    field2=`echo $test_addr |  cut -d '.' -f2` 
    field3=`echo $test_addr |  cut -d '.' -f3` 
    field4=`echo $test_addr |  cut -d '.' -f4`

    # Test 2: All fields are comprised only of numberical chars
    if ! [[ "$field1" =~ ^[0-9]+$ ]]; then
        return 21
    elif ! [[ "$field2" =~ ^[0-9]+$ ]]; then
        return 22
    elif ! [[ "$field3" =~ ^[0-9]+$ ]]; then
        return 23
    elif ! [[ "$field4" =~ ^[0-9]+$ ]]; then
        return 24
    fi

    # Test 3: All quads in range of [0, 255]
    if [[ $field1 -gt 255 ]]; then
	return 31
    elif [[ $field2 -gt 255 ]]; then
	return 32
    elif [[ $field3 -gt 255 ]]; then
	return 33
    elif [[ $field4 -gt 255 ]]; then
	return 34
    fi
    return 0
}


#-------------------------------------------------------
#  Part 4: Determine the operating system
#-------------------------------------------------------
if [ -d "/proc" ]; then
    OS="linux"
fi

#-------------------------------------------------------
#  Part 5: Ensure System Utilities are installed per OS
#-------------------------------------------------------
if [ "${OS}" = "linux" ]; then
    # Make sure the ifconfig utility is present
    command -v ifconfig >& /dev/null
    if [ $? != 0 ]; then
	echo "The required utility ifconfig is not found. Exit code 2."  > $ERRFILE
	exit 2
    fi
fi

if [ "${OS}" = "osx" ]; then
    # Make sure the networksetup utility is present
    command -v networksetup >& /dev/null
    if [ $? != 0 ]; then
	echo "The required utility networksetup is not found. Exit code 3." > $ERRFILE
	exit 3
    fi
fi

EXIT_CODE=0
#-------------------------------------------------------
#  Part 6: Clear results file and Write file header if desired
#-------------------------------------------------------
echo -n "" > $TMP_RESFILE
if [ "$HEADER" = "yes" ]; then
    echo "// IP addresses of currently active network interfaces.  " >> $TMP_RESFILE
    echo "// Produced automatically by the ipaddr_info.sh utility. " >> $TMP_RESFILE
fi

#-------------------------------------------------------
#  Part 7: Handle the MaxOS case
#-------------------------------------------------------
if [ "${OS}" = "osx" ]; then
    
    networksetup -getinfo "Belkin USB-C LAN" | fgrep "IP address:" | fgrep "." >& /dev/null
    if [ $? = 0 ]; then
	addr=`networksetup -getinfo "Belkin USB-C LAN" | fgrep "IP address" | fgrep "." | cut -d ' ' -f3`
	echo $addr " Belkin USB-C LAN"  >> $TMP_RESFILE
    fi

    networksetup -getinfo "Wi-Fi" | fgrep "IP address:" | fgrep "." >& /dev/null
    if [ $? = 0 ]; then
	addr=`networksetup -getinfo "Wi-Fi" | fgrep "IP address" | fgrep "." | cut -d ' ' -f3`
	echo $addr " Wi-Fi"  >> $TMP_RESFILE
    fi
    
    networksetup -getinfo "Ethernet" | fgrep "IP address:" | fgrep "." >& /dev/null
    if [ $? = 0 ]; then
	addr=`networksetup -getinfo "Ethernet" | fgrep "IP address" | fgrep "." | cut -d ' ' -f3`
	echo $addr " Ethernet"  >> $TMP_RESFILE
    fi
    
    networksetup -getinfo "Ethernet 1" | fgrep "IP address:" | fgrep "." >& /dev/null
    if [ $? = 0 ]; then
	addr=`networksetup -getinfo "Ethernet 1" | fgrep "IP address" | fgrep "." | cut -d ' ' -f3`
	echo $addr " Ethernet 1"  >> $TMP_RESFILE
    fi
    
    networksetup -getinfo "Ethernet 2" | fgrep "IP address:" | fgrep "." >& /dev/null
    if [ $? = 0 ]; then
	addr=`networksetup -getinfo "Ethernet 2" | fgrep "IP address" | fgrep "." | cut -d ' ' -f3`
	echo $addr " Ethernet 2"  >> $TMP_RESFILE
    fi
fi

#-------------------------------------------------------
#  Part 8: Handle the Linux general case
#-------------------------------------------------------
if [ "${OS}" = "linux" ]; then

    declare -a interfaces=()
    interfaces=( $(ifconfig | fgrep "RUNNING" | cut -d ':' -f1) )

    for iface in "${interfaces[@]}"
    do
	if [ $iface != "lo" ]; then
	    addr=`ifconfig $iface | fgrep "netmask" | awk '{print $2}' `
	    ipvalid $addr
	    if [ $? = 0 ]; then
		echo "$addr $iface"  >> $TMP_RESFILE
	    fi
	fi
    done
fi

#-------------------------------------------------------
#  Part 9: If the tmp file is different, install it. 
#  By building a tmp file, with a unique file name based 
#  on the PID, this gaurds against possibly several 
#  versions of this script running at nearly the same 
#  time, and possiblly all writing to the destinatin file.
#-------------------------------------------------------

diff $TMP_RESFILE $RESFILE >& /dev/null

if [ $? != 0 ]; then
    mv -f $TMP_RESFILE $RESFILE
else
    rm -f $TMP_RESFILE
fi

echo "all ok. Exit code 0." > $ERRFILE
exit 0

