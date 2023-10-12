#!/bin/bash 
#---------------------------------------------------------------
#   Script: mhash_tagall.sh
#   Author: Mike Benjamin
#   LastEd: May 2023
#-------------------------------------------------------------- 
#  Part 1: Define a convenience function for producing terminal
#          debugging/status output depending on the verbosity.
#-------------------------------------------------------------- 
vecho() { if [ "$VERBOSE" != "" ]; then echo "$ME: $1"; fi }

#---------------------------------------------------------------
#  Part 2: Set global var defaults
#---------------------------------------------------------------
ME=`basename "$0"`
DIR=$(pwd)
VERBOSE=""
JUST_SHOW_ALOGS=""
BROAD="no"  

rst=$(tput setaf 0)  # Reset
red=$(tput setaf 1)  # Red
grn=$(tput setaf 2)  # Green
ylw=$(tput setaf 3)  # Yellow
blu=$(tput setaf 4)  # Blue
pur=$(tput setaf 5)  # Purple
cyn=$(tput setaf 6)  # Cyan


#---------------------------------------------------------------
#  Part 3: Check for and handle command-line arguments
#---------------------------------------------------------------
for ARGI; do
    if [ "${ARGI}" = "--help" -o "${ARGI}" = "-h" ]; then
	echo "$ME [SWITCHES] [time_warp]                      "
	echo "  --help, -h         Show this help message     "
	echo "  --verbose, -v      Verbose output             "
	echo "  --broad, -b        Visit all IVP_MISSION_DIRS "
	echo "  --alogs, -g        Just show .alog files      "
	echo "                                                "
	echo "Synopsis:                                          "
	echo "   $ME will visit all directories found in the     "
	echo "   mission folder path defined by IVP_MISSION_DIRS "
	echo "   environment variable.                           "
	echo "   It will invoke the mhash.sh script in each dir. "
	exit 0
    elif [ "${ARGI}" = "--verbose" -o "${ARGI}" = "-v" ]; then
	VERBOSE="-v"
    elif [ "${ARGI}" = "--broad" -o "${ARGI}" = "-b" ]; then
	BROAD="yes"
    elif [ "${ARGI}" = "--alogs" -o "${ARGI}" = "-g" ]; then
	JUST_SHOW_ALOGS="--alogs"
    else
        echo "$ME: Bad arg:" $ARGI "Exit Code 1."
        exit 1
    fi
done

#---------------------------------------------------------------
#  Part 4: Visit each directory in IVP_MISSION_DIRS
#---------------------------------------------------------------
MISSION_DIRS="./"

if [ "${BROAD}" = "yes" ]; then
    MISSION_DIRS="./:"$IVP_MISSION_DIRS
fi

IFS=':' read -r -a mission_dir_array <<< "$MISSION_DIRS"

for mission_dir in "${mission_dir_array[@]}"; do
    vecho "${pur} Entering [$mission_dir] ${rst}"
    cd "$mission_dir" >& /dev/null

    for file in *; do
	vecho "Checking [$file]"
        if [[ $file == *.alog ]]; then
	    if [ "${JUST_SHOW_ALOGS}" != "" ]; then
		echo $PWD/$file
	    else
		vecho "${cyn} running mhash_tag.sh"
		mhash_tag.sh $VERBOSE
	    fi
        fi
    done

    for file in *; do
        if [ -d $file ]; then
	    vecho "subdir:[$file]"
	    cd $file >& /dev/null
	    mhash_tagall.sh $VERBOSE $JUST_SHOW_ALOGS
	    cd - >& /dev/null
        fi
    done
    cd - >& /dev/null
done
