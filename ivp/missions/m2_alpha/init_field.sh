#!/bin/bash 
#------------------------------------------------------------
#   Script: init_field.sh
#   Author: M.Benjamin
#   LastEd: Oct 26 2026
#------------------------------------------------------------
#  Part 1: A convenience function for producing terminal
#          debugging/status output depending on verbosity.
#------------------------------------------------------------
vecho() { if [ "$VERBOSE" != "" ]; then echo "$ME: $1"; fi }

#------------------------------------------------------------
#  Part 2: Set global variable default values
#------------------------------------------------------------
ME=`basename "$0"`
VEHICLE_AMT="1"
VERBOSE=""

#------------------------------------------------------------
#  Part 3: Check for and handle command-line arguments
#------------------------------------------------------------
for ARGI; do
    CMD_ARGS+=" ${ARGI}"
    if [ "${ARGI}" = "--help" -o "${ARGI}" = "-h" ]; then
	echo "$ME [OPTIONS] [time_warp]                      "
	echo "                                               "
	echo "Options:                                       "
	echo "  --amt=N            Num vehicles to launch    "
	echo "  --verbose, -v      Verbose, confirm values   "
	echo "  --rand, -r         Rand vehicle positions    "
	echo "                                               "
       exit 0;
    elif [ "${ARGI:0:6}" = "--amt=" ]; then
        VEHICLE_AMT="${ARGI#--amt=*}"
    elif [ "${ARGI}" = "--verbose" -o "${ARGI}" = "-v" ]; then
	VERBOSE=$ARGI
    else 
	echo "$ME: Bad Arg: $ARGI. Exit Code 1."
	exit 1
    fi
done

#------------------------------------------------------------
#  Part 5: Set starting vehicle properties (vp files)
#------------------------------------------------------------
vecho "Setting starting position, vnames, colors"

echo "x=0,y=-20,heading=180,speed=0"  >  vp_startpos.txt
echo "x=20,y=-20,heading=180,speed=0" >> vp_startpos.txt
echo "x=40,y=-20,heading=180,speed=0" >> vp_startpos.txt
echo "x=60,y=-20,heading=180,speed=0" >> vp_startpos.txt
echo "x=80,y=-20,heading=180,speed=0" >> vp_startpos.txt

echo "x=0,y=-150"  >  vp_endpos.txt
echo "x=20,y=-150" >> vp_endpos.txt
echo "x=40,y=-150" >> vp_endpos.txt
echo "x=60,y=-150" >> vp_endpos.txt
echo "x=80,y=-150" >> vp_endpos.txt

echo "abe" >  vp_names.txt
echo "ben" >> vp_names.txt
echo "cal" >> vp_names.txt
echo "deb" >> vp_names.txt
echo "eve" >> vp_names.txt

echo "yellow"      >  vp_colors.txt
echo "red"         >> vp_colors.txt
echo "dodger_blue" >> vp_colors.txt
echo "green"       >> vp_colors.txt
echo "purple"      >> vp_colors.txt

#------------------------------------------------------------
#  Part 7: If verbose, show file contents
#------------------------------------------------------------
if [ "${VERBOSE}" != "" ]; then
    echo "--------------------------------------"
    echo "VEHICLE_AMT = $VEHICLE_AMT"
    echo "--------------------------------------(pos/name)"
    echo "vp_positions.txt:"; cat  vp_positions.txt
    echo "vp_names.txt:";     cat  vp_names.txt
    echo -n "Hit any key to continue"
    read ANSWER
fi

exit 0
