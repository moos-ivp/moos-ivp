#!/bin/bash -e
 
CLEAN="no"

# By default, all code is built
# On Raspbian, by default, only min-robot code is built
BUILD_BOT_CODE_ONLY="OFF"

if [ -x "$(command -v lsb_release)" ]; then
    OS_INFO=`lsb_release -i -s`
    if [ "${OS_INFO}" = "Raspbian" ]; then
	BUILD_BOT_CODE_ONLY="ON"
    fi
fi

print_usage_and_exit()
{
    echo "build.sh [OPTIONS] [MAKE ARGS]                "
    echo "Options:                                      "
    echo "  --help, -h                                  "
    echo "  --minrobot, -m                              "
    echo "    Only build minimal robot apps             "
    echo "  --minrobotx, -mx                            "
    echo "    Override min-robot default on Raspbian    "
    echo "  --clean, -c, clean                          "
    echo "    Removes all build, bin, library files     "
    exit 1
}

for ARGI; do
    if [ "${ARGI}" = "--help" -o "${ARGI}" = "-h" ] ; then
        print_usage_and_exit;
    elif [ "${ARGI}" = "--clean" -o "${ARGI}" = "-c" ] ; then
        CLEAN="yes"
    elif [ "${ARGI}" = "clean" -o "${ARGI}" = "-clean" ] ; then
        CLEAN="yes"
    elif [ "${ARGI}" = "--minrobot" -o "${ARGI}" = "-m" ] ; then
        BUILD_BOT_CODE_ONLY="ON"
    elif [ "${ARGI}" = "--minrobotx" -o "${ARGI}" = "-mx" ] ; then
        BUILD_BOT_CODE_ONLY="OFF"
    fi
done

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
    ./build-moos.sh
    ./build-ivp.sh 
fi

cd "${INVOC_ABS_DIR}"

