#!/bin/bash 
#------------------------------------------------------------
#   Script: init_field.sh
#   Author: M.Benjamin
#   LastEd: Oct 26 2026
#------------------------------------------------------------
#  Part 1: A convenience function for producing terminal
#          debugging/status output depending on verbosity.
#          A convenience function for showing contents of file
#------------------------------------------------------------
vecho() { if [ "$VERBOSE" != "" ]; then echo "$ME: $1"; fi }
vpcat() { echo "$1: "; sed 's/^/  /' $1; }

#------------------------------------------------------------
#  Part 2: Set global variable default values
#------------------------------------------------------------
ME=`basename "$0"`
VERBOSE=""
VEHICLE_AMT="1"

#------------------------------------------------------------
#  Part 3: Check for and handle command-line arguments
#------------------------------------------------------------
for ARGI; do
    CMD_ARGS+=" ${ARGI}"
    if [ "${ARGI}" = "--help" -o "${ARGI}" = "-h" ]; then
	echo "$ME [OPTIONS]                                  "
	echo "                                               "
	echo "Options:                                       "
	echo "  --verbose, -v      Verbose, confirm values   "
	echo "  --amt=N            Num vehicles to launch    "
	echo "                                               "
       exit 0;
    elif [ "${ARGI}" = "--verbose" -o "${ARGI}" = "-v" ]; then
	VERBOSE=$ARGI
    elif [ "${ARGI:0:6}" = "--amt=" ]; then
        VEHICLE_AMT="${ARGI#--amt=*}"
    else 
	echo "$ME: Bad Arg: $ARGI. Exit Code 1."
	exit 1
    fi
done

#------------------------------------------------------------
#  Part 5: Set starting vehicle properties (vp files)
#------------------------------------------------------------
vecho "Setting starting position, vnames, colors"

pickpos --amt=$VEHICLE_AMT --hdg=180  \
	--polygon=-25,0:-25,-25:185,-25:185,0 > vp_startpos.txt

pickpos --amt=$VEHICLE_AMT   \
	--polygon=-25,-140:-25,-160:185,-160:185,-140 > vp_endpos.txt

pickpos --amt=$VEHICLE_AMT --vnames > vp_names.txt

pickpos --amt=$VEHICLE_AMT --colors > vp_colors.txt

#------------------------------------------------------------
#  Part 7: If verbose, show file contents
#------------------------------------------------------------
if [ "${VERBOSE}" != "" ]; then
    echo "--------------------------------------"
    echo "VEHICLE_AMT = $VEHICLE_AMT"
    echo "--------------------------------------(pos/name)"
    vpcat vp_startpos.txt
    vpcat vp_endpos.txt
    vpcat vp_names.txt
    vpcat vp_colors.txt
    read ANSWER
fi

exit 0
