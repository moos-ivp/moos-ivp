#!/bin/bash 
#---------------------------------------------------------
# Script: build-ivp.sh
#     By: Mike Benjamin
#---------------------------------------------------------
#  Part 1: Define global script variables
#---------------------------------------------------------

BUILD_DEBUG="yes"  
BUILD_OPTIM="yes"
CLEAN="no"
CMD_ARGS="-j$(getconf _NPROCESSORS_ONLN)"
BUILD_WITH_UTM="-DUSE_UTM=ON"

# By default, all code is built
BUILD_BOT_CODE_ONLY="OFF"
BUILD_GUI_CODE="ON"
FORCE_FULL_RASPI_BUILD=""

#---------------------------------------------------------
# Part 2: Handle Command Line Arguments
#---------------------------------------------------------
for ARGI; do
    if [ "${ARGI}" = "--help" -o "${ARGI}" = "-h" ]; then
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
	echo "  --utm_off, -u                               "
	echo "    Do not build wit UTM for Geodesy          "
	echo "  --minrobot, -m                              "
	echo "    Only build minimal robot apps             "
	echo "    (Even smaller subset than with --nogui)   "
	echo "  --minrobotx, -mx                            "
	echo "    Override min-robot default on Raspbian    "
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
	exit 0
    elif [ "${ARGI}" = "--nodebug" ]; then
        BUILD_DEBUG="no"
    elif [ "${ARGI}" = "--noopt" ]; then
        BUILD_OPTIM="no"
    elif [ "${ARGI}" = "--utm_off" -o "${ARGI}" = "-u" ]; then
        BUILD_WITH_UTM="-DUSE_UTM=OFF"
    elif [ "${ARGI}" = "--fast" -o "${ARGI}" = "-f" ]; then
        BUILD_DEBUG="no"
        BUILD_OPTIM="no"
    elif [ "${ARGI}" = "--clean" -o "${ARGI}" = "-c" ]; then
        CLEAN="yes"
    elif [ "${ARGI}" = "--nogui" -o "${ARGI}" = "-n" ]; then
        BUILD_GUI_CODE="OFF"
    elif [ "${ARGI}" = "--minrobot" -o "${ARGI}" = "-m" ]; then
        BUILD_BOT_CODE_ONLY="ON"
	BUILD_GUI_CODE="OFF"
    elif [ "${ARGI}" = "--minrobotx" -o "${ARGI}" = "-mx" ]; then
        FORCE_FULL_RASPI_BUILD="yes"
    else
	if [ "$CMD_ARGS" = "" ]; then
	    CMD_ARGS=$ARGI
	else
	    CMD_ARGS=$CMD_ARGS" "$ARGI
	fi
    fi
done


#-------------------------------------------------------------- 
#  Part 3: If this is Raspbian and minrobot not selected, and
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
	    BUILD_GUI_CODE="OFF"
	fi
    fi
fi


#---------------------------------------------------------
# Part 4: Set Compiler flags
#---------------------------------------------------------
CMAKE_CXX_FLAGS="-Wall -Wextra -Wno-unused-parameter "
CMAKE_CXX_FLAGS+="-Wno-missing-field-initializers -pedantic -fPIC "

# -Wno-psabi turns off warnings about ABI change between gcc 6 and 7.1
CMAKE_CXX_FLAGS+="-Wno-psabi "


if [ "${BUILD_DEBUG}" = "yes" ] ; then
    CMAKE_CXX_FLAGS=$CMAKE_CXX_FLAGS" -g"
fi
if [ "${BUILD_OPTIM}" = "yes" ] ; then
    CMAKE_CXX_FLAGS=$CMAKE_CXX_FLAGS" -Os"
fi

echo "Compiler flags: ${CMAKE_CXX_FLAGS}  " 


#---------------------------------------------------------
# Part 5
#---------------------------------------------------------
INVOC_ABS_DIR="$(pwd)"
SCRIPT_ABS_DIR="$(cd $(dirname "$0") && pwd -P)"

BLD_ABS_DIR="${SCRIPT_ABS_DIR}/build"
LIB_ABS_DIR="${SCRIPT_ABS_DIR}/lib"
BIN_ABS_DIR="${SCRIPT_ABS_DIR}/bin"
SRC_ABS_DIR="${SCRIPT_ABS_DIR}/ivp/src"

echo "Built files will be placed into these directories: "
echo "  Intermediate build files: ${BLD_ABS_DIR}         "
echo "  Libraries:                ${LIB_ABS_DIR}         "
echo "  Programs:                 ${BIN_ABS_DIR}         "

mkdir -p "${BLD_ABS_DIR}"
mkdir -p "${LIB_ABS_DIR}"
mkdir -p "${BIN_ABS_DIR}"

cd "${BLD_ABS_DIR}"

#---------------------------------------------------------
# Part 6: For back compatability, if user has environment var
#         IVP_BUILD_GUI_CODE set to "OFF" then honor it here
#         as if --nogui it were set on the command line
#---------------------------------------------------------

if [ "${IVP_BUILD_GUI_CODE}" = "OFF" ] ; then
    BUILD_GUI_CODE="OFF"
    echo "IVP GUI Apps will not be built since the   "
    echo "IVP_BUILD_GUI_CODE environment var is OFF  "
fi

echo "BUILD_GUI_CODE = ${BUILD_GUI_CODE} "

#---------------------------------------------------------
# Part 7: Invoke CMake
#---------------------------------------------------------
echo "Invoking cmake..."

echo "BUILD_BOT_CODE_ONLY: ${BUILD_BOT_CODE_ONLY}   "

cmake -DIVP_BUILD_GUI_CODE=${BUILD_GUI_CODE}               \
      -DFLTK_SKIP_FLUID=ON                                 \
      -DIVP_BUILD_BOT_CODE_ONLY=${BUILD_BOT_CODE_ONLY}     \
      -DIVP_LIB_DIRECTORY="${LIB_ABS_DIR}"                 \
      -DIVP_BIN_DIRECTORY="${BIN_ABS_DIR}"                 \
      -DCMAKE_CXX_FLAGS="${CMAKE_CXX_FLAGS}"               \
      ${BUILD_WITH_UTM}                                    \
      ${IVP_CMAKE_FLAGS}                                   \
      "${SRC_ABS_DIR}"

#---------------------------------------------------------
# Part 8: Invoke Make
#---------------------------------------------------------
echo "Invoking make ${CMD_ARGS}"

RESULT=0
if [ "${CLEAN}" = "yes" -o "${CMD_ARGS}" = "clean" ] ; then
    echo "CLEANING...."
    make clean
    RESULT=$?
    cd ${INVOCATION_ABS_DIR}
    rm -rf build/*
else
    echo "=============================" >> ~/.moos-ivp-args
    echo "1" ${CMD_ARGS} >> ~/.moos-ivp-args
    echo "2" ${BUILD_GUI_CODE} >> ~/.moos-ivp-args
    echo "3" ${BUILD_BOT_CODE_ONLY} >> ~/.moos-ivp-args
    echo "4" ${LIB_ABS_DIR} >> ~/.moos-ivp-args
    echo "5" ${BIN_ABS_DIR} >> ~/.moos-ivp-args
    echo "6" ${CMAKE_CXX_FLAGS} >> ~/.moos-ivp-args
    echo "7" ${BUILD_WITH_UTM} >> ~/.moos-ivp-args
    echo "8" ${IVP_CMAKE_FLAGS} >> ~/.moos-ivp-args
    make ${CMD_ARGS}
    RESULT=$?
fi

cd ${INVOC_ABS_DIR}

exit $RESULT
