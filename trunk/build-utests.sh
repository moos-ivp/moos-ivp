#!/bin/bash

BUILD_DEBUG="yes"  
BUILD_OPTIM="yes"
CLEAN="no"
CMD_ARGS="-j$(getconf _NPROCESSORS_ONLN)"

print_usage_and_exit()
{
    printf "build-utests.sh [OPTIONS] [MAKE ARGS]         \n"
    printf "Options:                                      \n"
    printf "  --help, -h                                  \n"
    printf "  --nodebug                                   \n"
    printf "    Do not include the -g compiler flag       \n"
    printf "  --noopt                                     \n"
    printf "    Do not include the -Os compiler flag      \n"
    printf "  --fast, -f                                  \n"
    printf "    Do not include the -Os, -g compiler flags \n"
    printf "  --clean, -c                                 \n"
    printf "    Invokes make clean and removes build/*    \n"
    printf "                                              \n"
    printf "By default, all code is built, and the debug and optimization  \n"
    printf "compiler flags are invoked.                                    \n"
    printf "                                                               \n"
    printf "Note: By default -jN is provided to make to utilize up to N    \n"
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
    else
	if [ "$CMD_ARGS" = "" ] ; then
	    CMD_ARGS=$ARGI
	else
	    CMD_ARGS=$CMD_ARGS" "$ARGI
	fi
    fi
done

################################################################################
CMAKE_CXX_FLAGS="-Wall -Wextra -Wno-unused-parameter -Wno-missing-field-initializers -pedantic -fPIC"
if [ "${BUILD_DEBUG}" = "yes" ] ; then
    CMAKE_CXX_FLAGS=$CMAKE_CXX_FLAGS" -g"
fi
if [ "${BUILD_OPTIM}" = "yes" ] ; then
    CMAKE_CXX_FLAGS=$CMAKE_CXX_FLAGS" -Os"
fi

printf "Compiler flags: ${CMAKE_CXX_FLAGS}  \n\n" 


################################################################################
INVOC_ABS_DIR="$(pwd)"
SCRIPT_ABS_DIR="$(cd $(dirname "$0") && pwd -P)"

BLD_ABS_DIR="${SCRIPT_ABS_DIR}/build-utests"
LIB_ABS_DIR="${SCRIPT_ABS_DIR}/lib"
BIN_ABS_DIR="${SCRIPT_ABS_DIR}/bin"
SRC_ABS_DIR="${SCRIPT_ABS_DIR}/ivp/src_unit_tests"

printf "Built files will be placed into these directories: \n"
printf "  Intermediate build files: ${BLD_ABS_DIR}         \n"
printf "  Libraries:                ${LIB_ABS_DIR}         \n"
printf "  Programs:                 ${BIN_ABS_DIR}       \n\n"

mkdir -p "${BLD_ABS_DIR}"
mkdir -p "${LIB_ABS_DIR}"
mkdir -p "${BIN_ABS_DIR}"

cd "${BLD_ABS_DIR}"

################################################################################
printf "Invoking cmake...\n"

cmake -DUTX_LIB_DIRECTORY="${LIB_ABS_DIR}"                 \
      -DUTX_BIN_DIRECTORY="${BIN_ABS_DIR}"                 \
      -DCMAKE_CXX_FLAGS="${CMAKE_CXX_FLAGS}"               \
      -DUSE_UTM=ON                                         \
      ${UTX_CMAKE_FLAGS}                                   \
      "${SRC_ABS_DIR}"

################################################################################
printf "Invoking make ${CMD_ARGS}\n"

if [ "${CLEAN}" = "yes" -o "${CMD_ARGS}" = "clean" ] ; then
    printf "CLEANING....\n"
    make clean
    cd ${INVOCATION_ABS_DIR}
    rm -rf build-utests/*
else
  make ${CMD_ARGS}
fi

cd ${INVOC_ABS_DIR}

