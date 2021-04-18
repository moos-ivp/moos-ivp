#!/bin/bash
#--------------------------------------------------------------
#    Script: info_os.sh
#    Author: Michael Benjamin
#      Date: September 2020
#  Synopsis: Gather operating system info for either a multi-line
#            output for a results file, or in a single-line output
#            suitable for using in a file name.
#--------------------------------------------------------------
#  Part 1: Set global vars
#--------------------------------------------------------------
TERSE="no"
FAKEOS=""

#--------------------------------------------------------------
#  Part 2: Handle Command Line args
#--------------------------------------------------------------
for ARGI; do
    if [ "${ARGI}" = "--help" -o "${ARGI}" = "-h" ]; then
	echo "info_date.sh [OPTIONS]                     "
	echo "  --help, -h       Show this help message  "
	echo "  --terse, -t      Produce terse output    "
	exit 0
    elif [ "${ARGI}" = "--terse" -o "${ARGI}" = "-t" ]; then
        TERSE="yes"
    elif [ "${ARGI:0:9}" = "--fakeos=" ] ; then
	FAKEOS="${ARGI#--fakeos=*}"
    else
	echo "info_os.sh: Bad Arg: "$ARGI
	exit 1
    fi
done

#--------------------------------------------------------------
#  Part 3: Get the Operating System Info
#--------------------------------------------------------------
OS=""
OS_MAJOR=""
# Use existence of /proc to indicate linux, else assume osx.
if [ ! -d "/proc" ] ; then
    OS=`sw_vers -productVersion`
    OS="OSX-"$OS
    OSMAJOR="${OS%.*}"
    # Check if release if first release, e.g, 10.15 before a 10.15.1
    if [[ $OSMAJOR != *"."* ]]; then
	OSMAJOR=$OS
    fi
fi
if [ "${OS}" = "" ]; then
    OS_DISTRIB_ID=`lsb_release -i -s`
    OS_RELEASE=`lsb_release -r -s`
    OS_CODENAME=`lsb_release -c -s`
    OS=$OS_DISTRIB_ID"-"$OS_RELEASE"-"$OS_CODENAME
    OSMAJOR=$OS_DISTRIB_ID"-"$OS_RELEASE
fi

if [ "${FAKEOS}" != "" ]; then
    OS=$FAKEOS
    OSMAJOR="${OS%-*}"
fi

#--------------------------------------------------------------
#  Part 5: output results
#--------------------------------------------------------------

SUFFIX="unknown"
if [ "${OSMAJOR}" = "OSX-10.15" ]; then
    SUFFIX="osx_ctl"
elif [ "${OSMAJOR}" = "OSX-10.14" ]; then
    SUFFIX="osx_moj"
elif [ "${OSMAJOR}" = "OSX-10.13" ]; then
    SUFFIX="osx_hsi"
elif [ "${OSMAJOR}" = "OSX-10.12" ]; then
    SUFFIX="osx_sie"
elif [ "${OSMAJOR}" = "OSX-10.11" ]; then
	SUFFIX="osx_cap"
	
elif [[ "${OS}" == *jessie* ]]; then
    SUFFIX="_ras_jes"
elif [[ "${OS}" == *stretch* ]]; then
    SUFFIX="_ras_str"
elif [[ "${OS}" == *buster* ]]; then
    SUFFIX="ras_bus"
elif [[ "${OS}" == *Ubuntu-16.04* ]]; then
    SUFFIX="ubu_164"
elif [[ "${OS}" == *Ubuntu-18.04* ]]; then
    SUFFIX="ubu_184"
elif [[ "${OS}" == *Ubuntu-19.04* ]]; then
    SUFFIX="ubu_194"
elif [[ "${OS}" == *Ubuntu-19.10* ]]; then
    SUFFIX="ubu_1910"
elif [[ "${OS}" == *Ubuntu-20.04* ]]; then
    SUFFIX="ubu_2004"
fi

if [ "${TERSE}" = "yes" ]; then
    echo $SUFFIX
    exit 0
fi
	
echo "ostype=$OS" 
echo "ostype_major=$OSMAJOR" 
echo "os_alias=$SUFFIX"
exit 0
