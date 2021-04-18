#!/bin/bash 
#----------------------------------------------------------
#  Script: xlaunch.sh
#  Author: Michael Benjamin
#  LastEd: August 2020
#----------------------------------------------------------
#  Part 1: Declare global var defaults
#----------------------------------------------------------
PARE_WINDOW=15
RMORIG="no"
ALL_OK="yes"
DIR="./"

#----------------------------------------------------------
#  Part 1B: Declare Paring variables
#----------------------------------------------------------
MARK_LIST="COLLISION,NEAR_MISS"
PARE_LIST="BHV_IPF,VIEW_SEGLIST,DESIRED_RUDDER,DESIRED_THRUST,"
PARE_LIST+="NAV_HEADING_OVER_GROUND,IVPHELM_SUMMARY,IVPHELM_CPU,"
PARE_LIST+="IVPHELM_CREATE_CPU,IVPHELM_LOOP_CPU"

HIT_LIST="*ITER_GAP,*ITER_LEN,PSHARE*,NODE_REPORT*,"
HIT_LIST+="LOGGER_DIRECTORY,APPCAST_REQ,DB_QOS,NODE_BROKER_ACK"
HIT_LIST+="DB_CLIENTS,IVPHELM_ALLSTOP_DEBUG,NODE_BROKER_PING_0,"

#=============================================================
# Part 2: Handle command line arguments
#=============================================================
for ARGI; do
    if [ "${ARGI}" = "--help" -o "${ARGI}" = "-h" ]; then
	echo "launch_wrapup.sh [SWITCHES] [PARE_WINDOW]      "
	echo "launch_wrapup.sh--help, -h                     " 
	echo "  --rmo         Remove Original Alog Files     " 
	echo "  --dir=DIR     Act on files in directory DIR  " 
	exit 0
    elif [ "${ARGI//[^0-9]/}" = "$ARGI" -a "$TIME_WARP" = 1 ]; then 
        PARE_WINDOW=$ARGI
    elif [ "${ARGI}" = "--rmo" ]; then
	RMORIG="yes"
    elif [ "${ARGI:0:6}" = "--dir=" ]; then
        DIR="${ARGI#--dir=*}"
    else 
        echo "launch_wrapup.sh Bad arg:" $ARGI " Exiting with code: 1"
        exit 1
    fi
done

#=============================================================
# Part 3: Execute the alogpare in all log sub-directories
#=============================================================
for dirs in $DIR/LOG*/ ; do 
    cd $dirs
    alogscan --rate_only LOG*.alog
    alogpare LOG*.alog name.alog --pare_window=$PARE_WINDOW \
	--markvars=$MARK_LIST --hitvars=$HIT_LIST --parevars=$PARE_LIST
    rc=$?
    if [[ $rc != 0 ]]; then
	echo "pare failed: $rc"; ALL_OK="no"
    else
	alogscan --rate_only *_pared.alog
    fi
    aloggrep -f *_pared.alog vname_encounters.alog ENCOUNTER_SUMMARY \
	     COLLISION_DETECT_PARAMS
    cd -
    echo "============================================================"
done

#=============================================================
# Part 4: Remove orig alogfiles if user wants, pares succeeded
#=============================================================
if [ "$ALL_OK" = "yes" -a "$RMORIG" = "yes" ]; then 
    echo Removing original alog files...
    rm -f LOG*/LOG*.alog
fi

#=============================================================
# Part 5: Produce right exit code depending on succes of pares 
#=============================================================
if [ "$ALL_OK" != "yes" ]; then 
    exit 1
fi

#-------------------------------------------------------
#  Part 6: Save the logs in a dedicated folder
#-------------------------------------------------------
RESULTS_DIR=`date "+%Y_%m_%d_____%H_%M"`"_alog_files"
mkdir $RESULTS_DIR
mv targ* *LOG* $RESULTS_DIR
echo "Completely done with xlaunch."
