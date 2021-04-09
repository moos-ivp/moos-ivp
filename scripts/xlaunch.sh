#!/bin/bash 
#----------------------------------------------------------
#  Script: xlaunch.sh
#  Author: Michael Benjamin
#  LastEd: August 2020
#----------------------------------------------------------
#  Part 1: Declare global var defaults
#----------------------------------------------------------
TIME_WARP=1
COMMUNITY="shoreside"
JUST_MAKE="no"
FLOW_DOWN_ARGS="--auto "

SEND="no"
RESFILE="no"
CLEAN="no"
FAKEOS=""

#-------------------------------------------------------
#  Part 2: Check for and handle command-line arguments
#-------------------------------------------------------
for ARGI; do
    if [ "${ARGI}" = "--help" -o "${ARGI}" = "-h" ] ; then
	echo "xlaunch.sh [OPTIONS] [time_warp]     "
	echo "Synopsis:                                                  " 
	echo "  Run from within a mission folder:                        "
	echo "    - Auto launch a mission                                " 
	echo "    - Poke the mission with start commands                 " 
	echo "    - Periodically connect to the mission to detect if     " 
	echo "      quit conditions are met.                             " 
	echo "    - After quit conditions met, check for results         " 
	echo "    - Bring down the mission                               " 
	echo "    - Optionally generate an outcome report file           " 
	echo "    - Optionally send the report file to a central server  " 
	echo "                                                           " 
	echo "Options:                                                   " 
	echo "  --help, -h         Show this help message                " 
	echo "  --just_make, -j    Just create targ files, no launch     " 
	echo "  --test=cnf_01      Config for variation_cnf_01.txt       " 
	echo "  --res, -r          Make a results file                   " 
	echo "  --send, -s         Make and send a results file          " 
	echo "  --clean, -c        Remove outcome file after OK send     " 
	echo "  --com=alpha        Name the community to poke            " 
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
    elif [ "${ARGI}" = "--res" -o "${ARGI}" = "-r" ]; then
        RESFILE="yes"
    elif [ "${ARGI}" = "--send" -o "${ARGI}" = "-s" ]; then
        RESFILE="yes"
        SEND="yes"
    elif [ "${ARGI}" = "--clean" -o "${ARGI}" = "-c" ]; then
        CLEAN="yes"
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
./launch.sh $FLOW_DOWN_ARGS $TIME_WARP
LEXIT_CODE=$?

if [ $LEXIT_CODE != 0 ]; then
    echo "The launch.sh cmd return non-zero exit code: " $LEXIT_CODE
    exit 1
fi
if [ ${JUST_MAKE} = "yes" ] ; then
    exit 0
fi

#-------------------------------------------------------
#  Part 4: Start the mission with the right pokes
#-------------------------------------------------------
echo "Poking/Starting the mission $TNUM in 4 seconds ...." 
sleep 4
if [ "${COMMUNITY}" = "shoreside" ]; then
    uPokeDB targ_shoreside.moos DEPLOY_ALL=true MOOS_MANUAL_OVERRIDE_ALL=false
else
    uPokeDB targ_$COMMUNITY.moos DEPLOY=true MOOS_MANUAL_OVERRIDE=false
fi

#-------------------------------------------------------
#  Part 5: Monitor mission, kill MOOS processes when done
#-------------------------------------------------------
while [ 1 ]; do 
    sleep 6
    if uQueryDB targ_$COMMUNITY.moos ; then 
        break;
    fi
    echo After uQueryDB, waiting to try again.
    echo Mission: $TNUM TimeWarp: $TIME_WARP
done

echo "Checking for Pass/Fail....."
rm -f .checkvars
uQueryDB --passfail -cv targ_$COMMUNITY.moos
QRESULT=$?

RESULT="pass"
if [ $QRESULT != 0 ]; then
    RESULT="fail"
fi
echo "Done Checking for Pass/Fail. Bringing down the mission..."
killall pAntler
sleep 4
echo "Done bringing down the mision."

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
echo "score = $RESULT"          >> $OUTFILE
echo "xlaunch = 1.0"            >> $OUTFILE
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



