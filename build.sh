#!/bin/sh

THIS_DIR=${PWD}

MOOS_PACKAGE_VERSION=7.0.1
MOOS_BUILD_NUM=1
MOOS_PACKAGE_DIR=${PWD}/MOOS/MOOSBin

IVP_PACKAGE_VERSION=0.1.0
IVP_BUILD_NUM=1
IVP_PACKAGE_DIR=${THIS_DIR:?}/ivp/src/bin

SCRATCH_DIRECTORY=/tmp/moos-ivp-scratch-dir

# If anything goes wrong, stop right away.
set -e

cmake                            \
   -DMOOS_VERSION=${MOOS_PACKAGE_VERSION:?}          \
   -DIVP_PACKAGE_VERSION=${IVP_PACKAGE_VERSION:?}   \
   -DFLTK_DIR=/usr/lib           \
   -DFLTK_INCLUDE_DIR=/usr/include \
   -DPYTHON_INCLUDE_PATH=/usr/include/python2.5 \
   -DBUILD_CONTROL_AND_NAV=ON    \
   -DBUILD_INSTRUMENTS=ON        \
   -DBUILD_TOOLS=ON              \
   -DBUILD_OCEAN_SIM=ON          \
   -DBUILD_MATLAB_TOOLS=OFF      \
   -DBUILD_LAND_INSTRUMENTS=ON   \
   -DBUILD_OCEAN_INSTRUMENTS=ON  \
   -DBUILD_UMVS=ON               \
   -DBUILD_UPB=ON                \
   ./

make

# For now, prevent people from accidentally building the Debian packages.
# I'll make this cleaner sometime later. -CJC
exit


# Make the Debian packages...
if [ ! -d ${SCRATCH_DIRECTORY:?} ]; 
then
   mkdir ${SCRATCH_DIRECTORY:?}
fi


cd ${THIS_DIR}/MOOS/BuildScripts/DebianPackaging/

sh ./make-debs \
   ${MOOS_PACKAGE_VERSION:?} \
   ${MOOS_BUILD_NUM:?} \
   ${THIS_DIR:?}/MOOS  \
   ${MOOS_PACKAGE_DIR:?} \
   ${SCRATCH_DIRECTORY:?} \
   --no-matlab


cd ${THIS_DIR}/ivp/scripts/debian-packaging

sh ./make-debs  \
   ${IVP_PACKAGE_VERSION:?}  \
   ${IVP_BUILD_NUM:?}  \
   ${MOOS_PACKAGE_VERSION:?}  \
   ${THIS_DIR:?}/ivp  \
   ${IVP_PACKAGE_DIR:?}  \
   ${SCRATCH_DIRECTORY:?}

cd ${THIS_DIR}
