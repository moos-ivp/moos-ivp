#!/bin/bash
 
CLEAN="no"

BUILD_BOT_CODE_ONLY="OFF"
FORCE_FULL_RASPI_BUILD=""

#-------------------------------------------------------------------
#  Part 1: Check for and handle command-line arguments
#-------------------------------------------------------------------
for ARGI; do
    if [ "${ARGI}" = "--help" -o "${ARGI}" = "-h" ]; then
	echo "build.sh [OPTIONS] [MAKE ARGS]                "
	echo "Options:                                      "
	echo "  --help, -h                                  "
	echo "  --minrobot, -m                              "
	echo "    Only build minimal robot apps             "
	echo "  --minrobotx, -mx                            "
	echo "    Override min-robot default on Raspbian    "
	echo "  --clean, -c, clean                          "
	echo "    Removes all build, bin, library files     "
	exit 0
    elif [ "${ARGI}" = "--clean" -o "${ARGI}" = "-c" ]; then
        CLEAN="yes"
    elif [ "${ARGI}" = "clean" -o "${ARGI}" = "-clean" ]; then
        CLEAN="yes"
    elif [ "${ARGI}" = "--minrobot" -o "${ARGI}" = "-m" ]; then
        BUILD_BOT_CODE_ONLY="ON"
    elif [ "${ARGI}" = "--minrobotx" -o "${ARGI}" = "-mx" ]; then
        FORCE_FULL_RASPI_BUILD="yes"
    fi
done

#-------------------------------------------------------------- 
#  Part 2: If this is Raspbian and minrobot not selected, and
#          no explicit override given with -mx, CONFIRM first
#-------------------------------------------------------------- 
command -v raspi-gpio
if [ "$?" = "0" -a "${BUILD_BOT_CODE_ONLY}" = "OFF" ]; then
    if [ ! "${FORCE_FULL_RASPI_BUILD}" = "yes" ]; then
	echo "Pi OS detected without --minrobotx or -mx selected."
	echo "[y] Continue with full build"
	echo "[M] Continue with minrobot build"
	echo -n "Continue? [y/M] "
	read ANSWER
	if [ ! "${ANSWER}" = "y" ]; then
	    BUILD_BOT_CODE_ONLY="ON"
	fi
    fi
fi

INVOC_ABS_DIR="$(pwd)"
SCRIPT_ABS_DIR="$(cd $(dirname "$0") && pwd -P)"

cd "${SCRIPT_ABS_DIR}"

if [ ${CLEAN} = "yes" ] ; then
    echo "=========================================" 
    echo "CLEANING both MOOS and IVP               " 
    echo "=========================================" 
    rm -rf build/* bin/* lib/* include/*
elif [ ${BUILD_BOT_CODE_ONLY} = "ON" ] ; then
    echo "===========================================================" 
    echo "BUILDING MOOS and IvP code in min-robot mode (no GUI Apps) " 
    echo "===========================================================" 
    ./build-moos.sh -m
    ./build-ivp.sh -m
else 
    echo "=========================================" 
    echo "BUILDING All MOOS and IvP code           " 
    echo "=========================================" 
    ./build-moos.sh -mx
    ./build-ivp.sh -mx 
fi

cd "${INVOC_ABS_DIR}"

