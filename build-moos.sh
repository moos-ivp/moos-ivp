#!/bin/sh

#THIS_DIR=${PWD}

MOOS_PACKAGE_VERSION=7.0.1
#MOOS_BUILD_NUM=1
#MOOS_PACKAGE_DIR=${PWD}/MOOS/MOOSBin

#SCRATCH_DIRECTORY=/tmp/moos-ivp-scratch-dir

# If anything goes wrong, stop right away.
set -e

cmake                                           \
   -DMOOS_VERSION=${MOOS_PACKAGE_VERSION:?}     \
   -DFLTK_DIR=/usr/lib                          \
   -DFLTK_INCLUDE_DIR=/usr/include              \
   -DPYTHON_INCLUDE_PATH=/usr/include/python2.5 \
   -DBUILD_CONTROL_AND_NAV=ON                   \
   -DBUILD_INSTRUMENTS=ON                       \
   -DBUILD_TOOLS=ON                             \
   -DBUILD_OCEAN_SIM=ON                         \
   -DBUILD_NEWMAT=OFF                           \
   -DBUILD_MATLAB_TOOLS=OFF                     \
   -DBUILD_LAND_INSTRUMENTS=OFF                 \
   -DBUILD_OCEAN_INSTRUMENTS=OFF                \
   -DBUILD_UMVS=ON                              \
   -DBUILD_UPB=ON                               \
   ./

make
