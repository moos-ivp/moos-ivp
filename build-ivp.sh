#!/bin/bash

BUILD_DEBUG="yes"  
BUILD_OPTIM="yes"
CLEAN="no"
CMD_ARGS=""
BUILD_GUI_CODE="ON"
BUILD_BOT_CODE_ONLY="OFF"


print_usage_and_exit()
{
    printf "build-ivp.sh [OPTIONS] [MAKE ARGS]            \n"
    printf "Options:                                      \n"
    printf "  --help, -h                                  \n"
    printf "  --nodebug                                   \n"
    printf "    Do not include the -g compiler flag       \n"
    printf "  --noopt                                     \n"
    printf "    Do not include the -O3 compiler flag      \n"
    printf "  --fast, -f                                  \n"
    printf "    Do not include the -O3, -g compiler flags \n"
    printf "  --nogui, -n                                 \n"
    printf "    Do not build GUI related apps             \n"
    printf "  --minrobot, -m                              \n"
    printf "    Only build minimal robot apps             \n"
    printf "    (Even smaller subset than with --nogui)   \n"
    printf "  --clean, -c                                 \n"
    printf "    Invokes make clean and removes build/*    \n"
    printf "                                              \n"
    printf "By default, all code is built, and the debug and optimization  \n"
    printf "compiler flags are invoked.                                    \n"
    printf "                                                               \n"
    printf "Note: By default -j12 is provided to make to utilize up to 12  \n"
    printf "      processors in the build. This can be overridden simply   \n"
    printf "      by using -j1 on the command line instead. This will give \n"
    printf "      more reasonable output if there should be a build error. \n"
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
        BUILD_GUI_CODE="no"
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

################################################################################
CMAKE_CXX_FLAGS="-Wall -Wno-long-long -pedantic -Wunreachable-code -Wmissing-declarations -fPIC"
if [ ${BUILD_DEBUG} = "yes" ] ; then
    CMAKE_CXX_FLAGS=$CMAKE_CXX_FLAGS" -g"
fi
if [ ${BUILD_OPTIM} = "yes" ] ; then
    CMAKE_CXX_FLAGS=$CMAKE_CXX_FLAGS" -O3"
fi

printf "Compiler flags: ${CMAKE_CXX_FLAGS}  \n\n" 


################################################################################
INVOCATION_ABS_DIR="`pwd`"

BLD_ABS_DIR="${INVOCATION_ABS_DIR}/build"
LIB_ABS_DIR="${INVOCATION_ABS_DIR}/lib"
BIN_ABS_DIR="${INVOCATION_ABS_DIR}/bin"
SRC_ABS_DIR="${INVOCATION_ABS_DIR}/ivp/src"

printf "Built files will be placed into these directories: \n"
printf "  Intermediate build files: ${BLD_ABS_DIR}         \n"
printf "  Libraries:                ${LIB_ABS_DIR}         \n"
printf "  Programs:                 ${BIN_ABS_DIR}       \n\n"

mkdir -p "${BLD_ABS_DIR}"
mkdir -p "${LIB_ABS_DIR}"
mkdir -p "${BIN_ABS_DIR}"

cd "${BLD_ABS_DIR}"

################################################################################
# For back compatability, if user has environment variable IVP_BUILD_GUI_CODE 
# set to "OFF" then honor it here as if --nogui were set on the command line

if [ ${IVP_BUILD_GUI_CODE} = "OFF" ] ; then
    BUILD_GUI_CODE="OFF"
    printf "IVP GUI Apps will not be built. IVP_BUILD_GUI_CODE env var is OFF\n"
fi

printf "BUILD_GUI_CODE = ${BUILD_GUI_CODE} \n"

################################################################################
printf "Invoking cmake...\n"

printf "BUILD_BOT_CODE_ONLY: ${BUILD_BOT_CODE_ONLY}   \n"

cmake -DIVP_BUILD_GUI_CODE=${BUILD_GUI_CODE}               \
      -DIVP_BUILD_BOT_CODE_ONLY=${BUILD_BOT_CODE_ONLY}     \
      -DIVP_LIB_DIRECTORY="${LIB_ABS_DIR}"                 \
      -DIVP_BIN_DIRECTORY="${BIN_ABS_DIR}"                 \
      -DCMAKE_CXX_FLAGS="${CMAKE_CXX_FLAGS}"               \
      -DUSE_UTM=ON                                         \
      ${IVP_CMAKE_FLAGS}                                   \
      "${SRC_ABS_DIR}"

################################################################################
printf "Invoking make ${CMD_ARGS}\n"

if [ "${CLEAN}" = "yes" -o "${CMD_ARGS}" = "clean" ] ; then
    printf "CLEANING....\n"
    make clean
    cd ${INVOCATION_ABS_DIR}
    rm -rf build/*
else
    make -j12 ${CMD_ARGS}
fi

cd ${INVOCATION_ABS_DIR}
