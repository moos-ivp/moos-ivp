#!/bin/sh

# Note by mikerb (Mar 8th 2008)
#
# The MOOS subdirectory is the exact directory distributed
# by Oxford. This script is used simply to facilitate the 
# switching off of modules that are not typically built in 
# local users of MOOS-IvP at MIT. 

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
   -DBUILD_GRAPHICAL_TOOLS=OFF                  \
   -DBUILD_MATLAB_TOOLS=OFF                     \
   -DMATLAB_DIR=/usr/local/matlab-2007a         \
   -DBUILD_LAND_INSTRUMENTS=OFF                 \
   -DBUILD_OCEAN_INSTRUMENTS=OFF                \
   -DBUILD_UMVS=ON                              \
   -DBUILD_UPB=ON                               \
   "${SRC_ABS_DIR}"

make

cd ${INVOCATION_ABS_DIR}
