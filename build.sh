#!/bin/bash
 
CLEAN="no"

BUILD_BOT_CODE_ONLY="OFF"
FORCE_FULL_RASPI_BUILD=""
MOOSGEODESY_USE_PROJ="${MOOSGEODESY_USE_PROJ:-ON}"
MOOSGEODESY_FETCH_PROJ="${MOOSGEODESY_FETCH_PROJ:-auto}"

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
        echo "  --with-proj                                 "
        echo "    Use Proj in MOOSGeodesy (default)         "
        echo "  --without-proj                              "
        echo "    Use original MOOSGeodesy code             "
        echo "  --fetch-proj=MODE                           "
        echo "    Fetch Proj: auto (default), on, or off    "
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
    elif [ "${ARGI}" = "--with-proj" ]; then
        MOOSGEODESY_USE_PROJ="ON"
    elif [ "${ARGI}" = "--without-proj" ]; then
        MOOSGEODESY_USE_PROJ="OFF"
    elif [[ "${ARGI}" == --fetch-proj=* ]]; then
        MOOSGEODESY_FETCH_PROJ="${ARGI#*=}"
        if [[ ! "${MOOSGEODESY_FETCH_PROJ}" =~ ^(auto|on|off)$ ]]; then
            echo "ERROR! --fetch-proj must be auto, on, or off"
            exit 1
        fi
    fi
done

#-------------------------------------------------------------- 
#  Part 2: If this is Raspbian and minrobot not selected, and
#          no explicit override given with -mx, CONFIRM first
#-------------------------------------------------------------- 
command -v raspi-config
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

RESULT=0
if [ ${CLEAN} = "yes" ] ; then
    echo "=========================================" 
    echo "CLEANING both MOOS and IVP               " 
    echo "=========================================" 
    rm -rf build/* bin/* lib/* include/*
elif [ ${BUILD_BOT_CODE_ONLY} = "ON" ] ; then
    echo "===========================================================" 
    echo "BUILDING MOOS and IvP code in min-robot mode (no GUI Apps) " 
    echo "===========================================================" 
    MOOSGEODESY_USE_PROJ="${MOOSGEODESY_USE_PROJ}" \
    MOOSGEODESY_FETCH_PROJ="${MOOSGEODESY_FETCH_PROJ}" \
        ./build-moos.sh -m && ./build-ivp.sh -m
    RESULT=$?
else 
    echo "=========================================" 
    echo "BUILDING All MOOS and IvP code           " 
    echo "=========================================" 
    MOOSGEODESY_USE_PROJ="${MOOSGEODESY_USE_PROJ}" \
    MOOSGEODESY_FETCH_PROJ="${MOOSGEODESY_FETCH_PROJ}" \
        ./build-moos.sh -mx && ./build-ivp.sh -mx
    RESULT=$?
fi

cd "${INVOC_ABS_DIR}"

exit ${RESULT}
