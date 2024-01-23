#!/bin/bash 
#---------------------------------------------------------------
#   Script: mhash_tag.sh
#   Author: Mike Benjamin
#   LastEd: May 2023
#-------------------------------------------------------------- 
#  Part 1: Define a convenience function for producing terminal
#          debugging/status output depending on the verbosity.
#-------------------------------------------------------------- 
vecho() { if [ "$VERBOSE" != "" ]; then echo "$BUF$ME: $1"; fi }

#---------------------------------------------------------------
#  Part 2: Set global var defaults
#---------------------------------------------------------------
ME=`basename "$0"`
VERBOSE=""
FORCE=""
LEVEL=0
BLD_NPOS=""

rst=$(tput setaf 0)  # Reset
red=$(tput setaf 1)  # Red
grn=$(tput setaf 2)  # Green
blu=$(tput setaf 4)  # Blue
pur=$(tput setaf 5)  # Purple
cyn=$(tput setaf 6)  # Cyan

#---------------------------------------------------------------
#  Part 3: Check for and handle command-line arguments
#---------------------------------------------------------------
for ARGI; do
    if [ "${ARGI}" = "--help" -o "${ARGI}" = "-h" ]; then
	echo "$ME [SWITCHES] [time_warp]                         "
	echo "  --help, -h        Show this help message         "
	echo "  --verbose, -v     Verbose output                 "
	echo "  --force, -f       Overwrite old .mhash file      "
	echo "  --npos, -n        Make nav position (npos) file  "
	echo "                                                   "
	echo "Synopsis:                                          "
	echo "  $ME will process the current directory and looks "
	echo "  for an alog file. It will make an .mhash file    "
	echo "  corresponding to the .alog file if it does not   "
	echo "  currently have one.                              "
	echo "Returns:                                           "
	echo "  0 if .mhash file created or existed previously   "
	echo "  1 if unable to create an .mhash file             "
	echo "                                                   "
	exit 0
    elif [ "${ARGI//[^0-9]/}" = "$ARGI" ]; then
        LEVEL=$ARGI
    elif [ "${ARGI}" = "--verbose" -o "${ARGI}" = "-v" ]; then
	VERBOSE="--verbose"
    elif [ "${ARGI}" = "--force" -o "${ARGI}" = "-f" ]; then
	FORCE="-f"
    elif [ "${ARGI}" = "--npos" -o "${ARGI}" = "-n" ]; then
        BLD_NPOS="yes"
    else
        echo "$ME: Bad arg:" $ARGI "Exit Code 1."
        exit 1
    fi
done
# create string of blanks with string length $LEVEL 
BUF=`printf %${LEVEL}s`

#---------------------------------------------------------------
#  Part 4: Build .mhash file if does not yet exist
#          The .mhash file should have ONE line with the following
#          format by example:
# 
#  mhash=230511-0724M-GREY-DEAL,odo=1887.2,duration=437,name=abe,
#  utc=1684424244.35,xhash=ABE-131S-000M

MHASH_FILE=""
for file in *; do
    if [[ $file == *.alog ]]; then
	FILE="${file%.*}"
	MHASH_FILE="${FILE}.mhash"
	
	if [ ! -e $MHASH_FILE ]; then
	    vecho "${blu} Alog file needs mhash: $file ${rst}"
	    alogmhash $file --all > ${MHASH_FILE}
	elif [ "${FORCE}" != "" ]; then
	    vecho "${blu} Overwriting mhash: $file ${rst}"
	    alogmhash $file --all > ${MHASH_FILE}
	else
	    vecho "${grn} mhash exists for: $file ${rst}"
	fi
    fi
done

if [ "${MHASH_FILE}" = "" ]; then
    vecho "${red} No .mhash file found or created. Exit 1. ${rst}"
    exit 1
fi

#---------------------------------------------------------------
#  Part 5: Build .npos file if does not yet exist
#          The .npos file will hold vehcle X/Y positions with 
#          timestamp suitable for plotting.

if [ "${BLD_NPOS}" = "yes" ]; then
    MHASH=`mhash_query.sh -m`
    XHASH=`mhash_query.sh -x`
    FHASH="${MHASH}-${XHASH}"
    NPOS_FILE="${FHASH}.npos"
    TGAP="--tgap=0.6"
    DGAP="--dgap=0.5"
    
    if [ ! -e $NPOS_FILE ]; then
	vecho "${blu} Alog file needs npos: ${NPOS_FILE} ${rst}"
	alognpos $VERBOSE *.alog $TGAP $DGAP "${NPOS_FILE}"
    elif [ "${FORCE}" != "" ]; then
	vecho "${blu} Overwriting mhash: ${NPOS_FILE} ${rst}"
	alognpos $VERBOSE *.alog $TGAP $DGAP "${NPOS_FILE}"
    else
	vecho "${grn} Alog file DOES NOT need npos: ${NPOS_FILE} ${rst}"
    fi
fi


exit 0
