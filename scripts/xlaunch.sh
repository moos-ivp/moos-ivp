#!/bin/bash 
#----------------------------------------------------------
#  Script: xlaunch.sh
#  Author: Michael Benjamin
#  LastEd: August 2020
#----------------------------------------------------------
#  Part 1: Declare global var defaults
#----------------------------------------------------------
TIME_WARP=1
COMMUNITY=""
JUST_MAKE="no"
UNIQUE=""
FLOW_DOWN_ARGS="--auto "
SEND="no"
RESFILE="no"
CLEAN="no"
DELAY="4"
FAKEOS=""
QUIET="no"
ALL_ARGS=""

#-------------------------------------------------------
#  Part 2: Check for and handle command-line arguments
#-------------------------------------------------------
for ARGI; do
    ALL_ARGS+=$ARGI" "
    if [ "${ARGI}" = "--help" -o "${ARGI}" = "-h" ]; then
	echo "xlaunch.sh [OPTIONS] [time_warp]     "
	echo "Synopsis:                                                    " 
	echo "  Run from within a mission folder:                          "
	echo "    - Auto launch a mission                                  " 
	echo "    - Poke the mission with start commands                   " 
	echo "    - Periodically connect to the mission to detect if       " 
	echo "      quit conditions are met.                               " 
	echo "    - After quit conditions met, check for results           " 
	echo "    - Bring down the mission                                 " 
	echo "    - Optionally generate an outcome report file             " 
	echo "    - Optionally send the report file to a central server    " 
	echo "                                                             " 
	echo "Options:                                                     " 
	echo "  --help, -h         Show this help message                  " 
	echo "  --just_make, -j    Just create targ files, no launch       " 
	echo "  --test=cnf_01      Config for variation_cnf_01.txt         " 
	echo "  --res, -r          Make a results file                     " 
	echo "  --send, -s         Make and send a results file            " 
	echo "  --clean, -c        Remove outcome file after OK send       " 
	echo "  --quiet, -q        Quiet uQueryDB, uPokeDB                 " 
	echo "  --com=alpha        Name the community to poke              " 
	echo "  --delay=<secs>     Delay N secs before launch. Default is 4" 
	echo "  --unique, -u       Pseudo unique proc name on each uQueryDB" 
	exit 0;
    elif [ "${ARGI//[^0-9]/}" = "$ARGI" -a "$TIME_WARP" = 1 ]; then 
	TIME_WARP=$ARGI
    elif [ "${ARGI}" = "--just_make" -o "${ARGI}" = "-j" ]; then
	FLOW_DOWN_ARGS+="${ARGI} "
	JUST_MAKE="yes"
    elif [ "${ARGI:0:6}" = "--com=" ]; then
        COMMUNITY="${ARGI#--com=*}"
    elif [ "${ARGI:0:7}" = "--test=" ]; then
	FLOW_DOWN_ARGS+="${ARGI} "
        TNUM="${ARGI#--test=*}"
    elif [ "${ARGI:0:8}" = "--delay=" ]; then
	DELAY="${ARGI#--delay=*}"
    elif [ "${ARGI}" = "--res" -o "${ARGI}" = "-r" ]; then
        RESFILE="yes"
    elif [ "${ARGI}" = "--send" -o "${ARGI}" = "-s" ]; then
        RESFILE="yes"
        SEND="yes"
    elif [ "${ARGI}" = "--clean" -o "${ARGI}" = "-c" ]; then
        CLEAN="yes"
    elif [ "${ARGI}" = "--quiet" -o "${ARGI}" = "-q" ]; then
        QUIET="yes"
    elif [ "${ARGI}" = "--unique" -o "${ARGI}" = "-u" ]; then
        UNIQUE=$ARGI
    elif [ "${ARGI}" = "--pi" ]; then
        FAKEOS="--fakeos=Raspbian-10-buster"
    elif [ "${ARGI}" = "--ubu" ]; then
        FAKEOS="--fakeos=Ubuntu-16.04-xenial"
    else 
	FLOW_DOWN_ARGS+="${ARGI} "
    fi
done

#-------------------------------------------------------
#  Part 3: Launch mission
#-------------------------------------------------------
echo "=================================================="
echo "xlaunch.sh: $ALL_ARGS"
echo "=================================================="
echo -n "Part 1: Launching the Mission... "

if [ "${QUIET}" = "yes" ]; then
    ./launch.sh $FLOW_DOWN_ARGS $TIME_WARP >& /dev/null
else
    ./launch.sh $FLOW_DOWN_ARGS $TIME_WARP
fi
LEXIT_CODE=$?

if [ $LEXIT_CODE != 0 ]; then
    echo "The launch.sh cmd return non-zero exit code: " $LEXIT_CODE
    echo "FLOW_DOWN_ARGS: "$FLOW_DOWN_ARGS
    exit 1
fi
if [ ${JUST_MAKE} = "yes" ]; then
    exit 0
fi
echo "DONE"

#-------------------------------------------------------
#  Part 4: Start the mission with the right pokes
#-------------------------------------------------------
if [ ! -f "targ_$COMMUNITY.moos" ]; then
    echo "targ_$COMMUNITY unfound. Use --com=<community>. Exit Code 9."
    exit 9
fi

echo -n "Part 2: Poking/Starting mission $TNUM in $DELAY seconds... "  
sleep $DELAY

echo "QUIET: ${QUIET}"
if [ "${QUIET}" = "yes" ]; then
    uPokeDB targ_$COMMUNITY.moos --cache >& /dev/null
else
    uPokeDB targ_$COMMUNITY.moos --cache
fi
echo "DONE"

#-------------------------------------------------------
#  Part 5: Monitor mission, kill MOOS processes when done
#-------------------------------------------------------
echo "Part 3: Query the mission $TNUM for halt conditions"
while [ 1 ]; do 
    echo -n "Waiting to Query... "
    sleep 6
    rm -f .checkvars
    if [ "${QUIET}" = "yes" ]; then
	uQueryDB $UNIQUE targ_$COMMUNITY.moos >& /dev/null
    else
	uQueryDB $UNIQUE targ_$COMMUNITY.moos
    fi

    if [ "$?" = 0 ]; then 
        break;
    fi
    echo "Not Done"
done

echo "DONE"
echo -n "Part 4: Bringing down the mission... "
killall pAntler
sleep 4
echo "DONE"

#=======================================================
#  Part 6: If not building results file, WE'RE DONE   == 
#=======================================================
if [ ${RESFILE} = "no" ]; then
    exit 0
fi

TGRP=${TNUM%_*}

OUTFILE=`info_date.sh -t`
OUTFILE+="_outcome_"$TNUM"_"
OUTFILE+=`info_os.sh $FAKEOS -t`
OUTFILE+=".txt"
UTC_TIME=`date +%s`
LOCATION="ivp${PWD#*/ivp}"

#--------------------------------------------------------------
#  Part 7: Produce info lines to output file
#--------------------------------------------------------------
echo "filename = $OUTFILE"       > $OUTFILE
echo "test_group = ${TGRP}"     >> $OUTFILE 
echo "test_name = ${TNUM}"      >> $OUTFILE 
echo "utc_time = ${UTC_TIME}"   >> $OUTFILE
echo "host = $HOSTNAME"         >> $OUTFILE 
echo "warp = $TIME_WARP"        >> $OUTFILE
echo "xlaunch = 1.1"            >> $OUTFILE
echo "location = $LOCATION"     >> $OUTFILE

echo "run = ./launch.sh --test=${TNUM} <warp>" >> $OUTFILE

info_date.sh        >> $OUTFILE
info_os.sh $FAKEOS  >> $OUTFILE
cat .checkvars      >> $OUTFILE

tagrep "variation_$TNUM.txt" thumb -f --hdr >> $OUTFILE
tagrep "variation_$TNUM.txt" run   -f --hdr >> $OUTFILE
tagrep "variation_$TNUM.txt" link  -f --hdr >> $OUTFILE

tagrep "variation_$TNUM.txt" synopsis -k -n >> $OUTFILE

tagrep "README" group-synopsis -k  >> $OUTFILE
tagrep "README" group-thumb -k  >> $OUTFILE

#--------------------------------------------------------------
#  Part 8: If also sending to server, send away!
#--------------------------------------------------------------
if [ "${SEND}" = "yes" ]; then
    ddj.sh --aut $OUTFILE
    SEND_RESULT=$?
    if [ $SEND_RESULT = 0 ]; then
	if [ "${CLEAN}" = "yes" ]; then
	    rm -f $OUTFILE
	fi
    fi
fi
