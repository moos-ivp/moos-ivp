#!/bin/sh

# Note by mikerb (Mar 8th 2008)
#
# The MOOS subdirectory is the exact directory distributed
# by Oxford. This script is used simply to facilitate the 
# switching off of modules that are not typically built in 
# local users of MOOS-IvP at MIT. 
#

INVOCATION_ABS_DIR="`pwd`"

# We want absolute pathnames, which require a little extra work to get...
SCRIPT_DIR="`dirname ${0}`"
cd "${SCRIPT_DIR}"
SCRIPT_ABS_DIR=`pwd`

SRC_ABS_DIR="${SCRIPT_ABS_DIR}/MOOS"
TARGET_ABS_DIR="${INVOCATION_ABS_DIR}/MOOS"

if [ "${INVOCATION_ABS_DIR}" != "${SCRIPT_ABS_DIR}" ]; then
   if [ -f "${SRC_ABS_DIR}/CMakeCache.txt" ]; then
      echo ""
      echo "The directory ${SRC_ABS_DIR} contains a file named CMakeCache.txt"
      echo "You're trying to perform an out-of-source build, but that's "
      echo "not possible if you have CMakeCache.txt files in your source "
      echo "tree."
      echo ""
      echo "If you really want to do an out-of-source build, you should delete"
      echo "all CMakeCache.txt files from your source tree."
      echo ""
      return 1
   fi
fi

mkdir -p "${TARGET_ABS_DIR}"
cd "${TARGET_ABS_DIR}"

################################################################################

echo "MOOS_BUILD_GRAPHICAL_TOOLS='"${MOOS_BUILD_GRAPHICAL_TOOLS}"'"

case "${MOOS_BUILD_GRAPHICAL_TOOLS}" in
   'ON'|'YES'|'TRUE'|'' )
      CLEANED_MOOS_BUILD_GRAPHICAL_TOOLS=ON
      ;;
   'OFF'|'NO'|'FALSE' )
      CLEANED_MOOS_BUILD_GRAPHICAL_TOOLS=OFF
      ;;
   *)
      echo 'Invalid value for the environment variable "MOOS_BUILD_GRAPHICAL_TOOLS"'
      echo '  If you set it at all, please set it to "ON" or "OFF"'
      return 1
      ;;
esac

echo ""
echo "Environment variable 'MOOS_BUILD_GRAPHICAL_TOOLS' = '"${MOOS_BUILD_GRAPHICAL_TOOLS}"'"
echo "   So will invoke cmake with -DBUILD_GRAPHICAL_TOOLS=${CLEANED_MOOS_BUILD_GRAPHICAL_TOOLS}"

################################################################################

case "${MOOS_BUILD_MATLAB_TOOLS}" in
   'ON'|'YES'|'TRUE')
      CLEANED_MOOS_BUILD_MATLAB_TOOLS=ON
      ;;
   'OFF'|'NO'|'FALSE'|'')
      CLEANED_MOOS_BUILD_MATLAB_TOOLS=OFF
      ;;
   *)
      echo 'Invalid value for the environment variable "MOOS_BUILD_MATLAB_TOOLS"'
      echo '  If you set it at all, please set it to "ON" or "OFF"'
      return 1
      ;;
esac

echo ""
echo "Environment variable 'MOOS_BUILD_MATLAB_TOOLS' = '"${MOOS_BUILD_MATLAB_TOOLS}"'"
   echo "   So will invoke cmake with -DBUILD_MATLAB_TOOLS=${CLEANED_MOOS_BUILD_MATLAB_TOOLS}"

################################################################################

echo ""
echo "Environment variable MOOS_MATLAB_DIR = '"${MOOS_MATLAB_DIR}"'"
echo "   So will invoke cmake with -DMATLAB_DIR=${MOOS_MATLAB_DIR}"

################################################################################

echo ""
echo "Invoking cmake..."
echo ""

# Setup C and C++ Compiler flags for Mac and Linux. 
if [ "`uname`" == "Darwin" ] ; then
  echo "Building MOOS for Apple"
  # On Mac OS X, we need to force MOOS to compile in 32-bit due to FLTK.
  MOOS_C_FLAGS="-m32"
  MOOS_CXX_FLAGS="-fPIC -m32"
else
  echo "Building MOOS for Linux"
  MOOS_CXX_FLAGS="-fPIC"
fi


cmake                                           \
   -DFLTK_DIR=/usr/lib                          \
   -DFLTK_INCLUDE_DIR=/usr/include              \
   -DPYTHON_INCLUDE_PATH=/usr/include/python2.5 \
   -DBUILD_CONTROL_AND_NAV=ON                   \
   -DBUILD_INSTRUMENTS=ON                      \
   -DDB_HAS_HTTP=ON                             \
   -DBUILD_TOOLS=ON                             \
   -DBUILD_OCEAN_SIM=ON                         \
   -DBUILD_NAVIGATION=OFF                       \
   -DBUILD_HELM=OFF                             \
   -DBUILD_NEWMAT=OFF                           \
   -DBUILD_GRAPHICAL_TOOLS=${CLEANED_MOOS_BUILD_GRAPHICAL_TOOLS} \
   -DBUILD_MATLAB_TOOLS=${CLEANED_MOOS_BUILD_MATLAB_TOOLS} \
   -DMATLAB_DIR=${MOOS_MATLAB_DIR}          \
   -DBUILD_LAND_INSTRUMENTS=OFF                 \
   -DBUILD_OCEAN_INSTRUMENTS=OFF                \
   -DBUILD_UMVS=ON                              \
   -DBUILD_UPB=ON                               \
   -DCMAKE_CXX_FLAGS="${MOOS_CXX_FLAGS}" \
   -DCMAKE_C_FLAGS="${MOOS_C_FLAGS}"
   "${SRC_ABS_DIR}"

echo ""
echo "Invoking make..."
echo ""

make $@

cd ${INVOCATION_ABS_DIR}
