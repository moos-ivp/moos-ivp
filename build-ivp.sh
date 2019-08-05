#!/bin/bash

BUILD_DEBUG="yes"  
BUILD_OPTIM="yes"
CLEAN="no"
CMD_ARGS="-j$(getconf _NPROCESSORS_ONLN)"
BUILD_GUI_CODE="ON"
BUILD_BOT_CODE_ONLY="OFF"

print_usage_and_exit()
{
    echo "build-ivp.sh [OPTIONS] [MAKE ARGS]            "
    echo "Options:                                      "
    echo "  --help, -h                                  "
    echo "  --nodebug                                   "
    echo "    Do not include the -g compiler flag       "
    echo "  --noopt                                     "
    echo "    Do not include the -Os compiler flag      "
    echo "  --fast, -f                                  "
    echo "    Do not include the -Os, -g compiler flags "
    echo "  --nogui, -n                                 "
    echo "    Do not build GUI related apps             "
    echo "  --minrobot, -m                              "
    echo "    Only build minimal robot apps             "
    echo "    (Even smaller subset than with --nogui)   "
    echo "  --clean, -c                                 "
    echo "    Invokes make clean and removes build/*    "
    echo "                                              "
    echo "By default, all code is built, and the debug and   "
    echo "optimization compiler flags are invoked.           "
    echo "                                                   "
    echo "Note:                                              "
    echo "  By default -jN is provided to make to utilize up to N    "
    echo "  processors in the build. This can be overridden simply   "
    echo "  by using -j1 on the command line instead. This will give "
    echo "  more reasonable output if there should be a build error. "
    exit 1
}

for ARGI; do
    if [ "${ARGI}" = "--help" -o "${ARGI}" = "-h" ] ; then
        print_usage_and_exit;
    elif [ "${ARGI}" = "--nodebug" ] ; then
        BUILD_DEBUG="no"
    elif [ "${ARGI}" = "--noopt" ] ; then
        BUILD_OPTIM="no"
    elif [ "${ARGI}" = "--fast" -o "${ARGI}" = "-f" ] ; then
        BUILD_DEBUG="no"
        BUILD_OPTIM="no"
    elif [ "${ARGI}" = "--clean" -o "${ARGI}" = "-c" ] ; then
        CLEAN="yes"
    elif [ "${ARGI}" = "--nogui" -o "${ARGI}" = "-n" ] ; then
        BUILD_GUI_CODE="OFF"
    elif [ "${ARGI}" = "--minrobot" -o "${ARGI}" = "-m" ] ; then
        BUILD_BOT_CODE_ONLY="ON"
	BUILD_GUI_CODE="OFF"
    else
	if [ "$CMD_ARGS" = "" ] ; then
	    CMD_ARGS=$ARGI
	else
	    CMD_ARGS=$CMD_ARGS" "$ARGI
	fi
    fi
done

#########################################################################
CMAKE_CXX_FLAGS="-Wall -Wextra -Wno-unused-parameter -Wno-missing-field-initializers -pedantic -fPIC"
if [ "${BUILD_DEBUG}" = "yes" ] ; then
    CMAKE_CXX_FLAGS=$CMAKE_CXX_FLAGS" -g"
fi
if [ "${BUILD_OPTIM}" = "yes" ] ; then
    CMAKE_CXX_FLAGS=$CMAKE_CXX_FLAGS" -Os"
fi

echo "Compiler flags: ${CMAKE_CXX_FLAGS}  " 


#########################################################################
INVOC_ABS_DIR="$(pwd)"
SCRIPT_ABS_DIR="$(cd $(dirname "$0") && pwd -P)"

BLD_ABS_DIR="${SCRIPT_ABS_DIR}/build"
LIB_ABS_DIR="${SCRIPT_ABS_DIR}/lib"
BIN_ABS_DIR="${SCRIPT_ABS_DIR}/bin"
SRC_ABS_DIR="${SCRIPT_ABS_DIR}/ivp/src"

echo "Built files will be placed into these directories: "
echo "  Intermediate build files: ${BLD_ABS_DIR}         "
echo "  Libraries:                ${LIB_ABS_DIR}         "
echo "  Programs:                 ${BIN_ABS_DIR}       \n"

mkdir -p "${BLD_ABS_DIR}"
mkdir -p "${LIB_ABS_DIR}"
mkdir -p "${BIN_ABS_DIR}"

cd "${BLD_ABS_DIR}"

########################################################################
# For back compatability, if user has environment variable
# IVP_BUILD_GUI_CODE set to "OFF" then honor it here as if --nogui
# were set on the command line

if [ "${IVP_BUILD_GUI_CODE}" = "OFF" ] ; then
    BUILD_GUI_CODE="OFF"
    echo "IVP GUI Apps will not be built. IVP_BUILD_GUI_CODE env var is OFF"
fi

echo "BUILD_GUI_CODE = ${BUILD_GUI_CODE} "

########################################################################
echo "Invoking cmake..."

echo "BUILD_BOT_CODE_ONLY: ${BUILD_BOT_CODE_ONLY}   "

cmake -DIVP_BUILD_GUI_CODE=${BUILD_GUI_CODE}               \
      -DIVP_BUILD_BOT_CODE_ONLY=${BUILD_BOT_CODE_ONLY}     \
      -DIVP_LIB_DIRECTORY="${LIB_ABS_DIR}"                 \
      -DIVP_BIN_DIRECTORY="${BIN_ABS_DIR}"                 \
      -DCMAKE_CXX_FLAGS="${CMAKE_CXX_FLAGS}"               \
      -DUSE_UTM=ON                                         \
      ${IVP_CMAKE_FLAGS}                                   \
      "${SRC_ABS_DIR}"

########################################################################
echo "Invoking make ${CMD_ARGS}"

RESULT=0
if [ "${CLEAN}" = "yes" -o "${CMD_ARGS}" = "clean" ] ; then
    echo "CLEANING...."
    make clean
    RESULT=$?
    cd ${INVOCATION_ABS_DIR}
    rm -rf build/*
else
    make ${CMD_ARGS}
    RESULT=$?
fi

cd ${INVOC_ABS_DIR}

exit $RESULT
