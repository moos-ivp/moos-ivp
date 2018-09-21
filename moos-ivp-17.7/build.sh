#!/bin/bash

CLEAN="no"
BUILD_BOT_CODE_ONLY="OFF"

print_usage_and_exit()
{
    printf "build.sh [OPTIONS] [MAKE ARGS]            \n"
    printf "Options:                                      \n"
    printf "  --help, -h                                  \n"
    printf "  --minrobot, -m                              \n"
    printf "    Only build minimal robot apps             \n"
    printf "  --clean, -c                                 \n"
    printf "    Invokes make clean and removes build/*    \n"
    exit 1
}

for ARGI; do
    if [ "${ARGI}" = "--help" -o "${ARGI}" = "-h" ] ; then
        print_usage_and_exit;
    elif [ "${ARGI}" = "--clean" -o "${ARGI}" = "-c" ] ; then
        CLEAN="yes"
    elif [ "${ARGI}" = "--minrobot" -o "${ARGI}" = "-m" ] ; then
        BUILD_BOT_CODE_ONLY="ON"
    fi
done

INVOC_ABS_DIR="$(pwd)"
SCRIPT_ABS_DIR="$(cd $(dirname "$0") && pwd -P)"

cd "${SCRIPT_ABS_DIR}"

if [ ${CLEAN} = "yes" ] ; then
    printf "=========================================\n" 
    printf "CLEANING both MOOS and IVP               \n" 
    printf "Note: You may have to manually clean MOOS\n" 
    printf "=========================================\n" 
    ./build-moos.sh --clean
    ./build-ivp.sh --clean
elif [ ${BUILD_BOT_CODE_ONLY} = "ON" ] ; then
    printf "===========================================================\n" 
    printf "BUILDING MOOS and IvP code in min-robot mode (no GUI Apps) \n" 
    printf "===========================================================\n" 
    ./build-moos.sh -m
    ./build-ivp.sh -m
else 
    printf "=========================================\n" 
    printf "BUILDING All MOOS and IvP code           \n" 
    printf "=========================================\n" 
    ./build-moos.sh
    ./build-ivp.sh 
fi

cd "${INVOC_ABS_DIR}"

