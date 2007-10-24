#!/bin/sh

set -e 

SCRIPT_DIR=../../scripts/util/create-subdir-cmake-files/

. ${SCRIPT_DIR}/util.sh

#===============================================================================

print_usage() {
   echo `basename ${0}`
   echo
   echo "Writes the CMakeLists.txt file for each subdirectory."
   echo "This script uses built-in information about what subdirectories should "
   echo "exist, and how each subdirectory's CMakeLists.txt file should look."
}

#===============================================================================

parse_and_validate_cmd_line() {
   if [ "$#" -ne "0" ]; then
      print_usage
      exit 1
   fi 
   
   SRC_DIR=./

   if [ ! -f ${SCRIPT_DIR}/simple-lib-template.txt ]; then
      echo "I should be able to find the file ${SCRIPT_DIR}/simple-lib-template.txt, "
      echo "but I can't."
      echo ""
      echo "You're supposed to run this script from it's own directory."
      exit 1
   fi

   if [ ! -f ${SCRIPT_DIR}/simple-app-template.txt ]; then
      echo "I should be able to find the file ${SCRIPT_DIR}/simple-app-template.txt, "
      echo "but I can't."
      echo ""
      echo "You're supposed to run this script from it's own directory."
      exit 1
   fi
}

#===============================================================================
# Main code...
#===============================================================================

parse_and_validate_cmd_line $*

#-------------------------------------------------------------------------------

SUBDIR=iBatteryMonitor \
PROGNAME=iBatteryMonitor \
IVP_BUILD_BY_DEFAULT=ON \
IVP_INSTALL_COMPONENT=ivp-tools \
LINK_LIBS="
   MOOS
   MOOSGen
   anrp_util
   m
   pthread" \
simple_app_cmake_file

SUBDIR=iCTDSBE49 \
PROGNAME=iCTDSBE49 \
IVP_BUILD_BY_DEFAULT=OFF \
IVP_INSTALL_COMPONENT=ivp-tools \
LINK_LIBS="
   MOOS
   MOOSGen
   anrp_util
   m
   pthread" \
simple_app_cmake_file

SUBDIR=iOS5000 \
PROGNAME=iOS5000 \
IVP_BUILD_BY_DEFAULT=OFF \
IVP_INSTALL_COMPONENT=ivp-tools \
LINK_LIBS="
   MOOS
   MOOSGen
   anrp_util
   m
   pthread" \
simple_app_cmake_file

SUBDIR=iPWMController \
PROGNAME=iPWMController \
IVP_INSTALL_COMPONENT=ivp-core \
IVP_BUILD_BY_DEFAULT=ON \
LINK_LIBS="
   MOOS
   MOOSGen
   anrp_util
   m
   pthread" \
simple_app_cmake_file

SUBDIR=iSerialNMEA \
PROGNAME=iSerialNMEA \
IVP_BUILD_BY_DEFAULT=ON \
IVP_INSTALL_COMPONENT=ivp-tools \
LINK_LIBS="
   MOOS
   MOOSGen
   MOOSUtility
   mbutil
   m
   pthread" \
simple_app_cmake_file

SUBDIR=iWinch \
PROGNAME=iWinch \
IVP_BUILD_BY_DEFAULT=ON \
IVP_INSTALL_COMPONENT=ivp-tools \
LINK_LIBS="
   MOOS
   MOOSGen
   anrp_util
   m
   pthread" \
simple_app_cmake_file

SUBDIR=iWinchCtl \
PROGNAME=iWinchCtl \
IVP_BUILD_BY_DEFAULT=ON \
IVP_INSTALL_COMPONENT=ivp-tools \
LINK_LIBS="
   MOOS
   MOOSGen
   anrp_util
   m
   pthread" \
simple_app_cmake_file

SUBDIR=iWinchSim \
PROGNAME=iWinchSim \
IVP_BUILD_BY_DEFAULT=ON \
IVP_INSTALL_COMPONENT=ivp-tools \
LINK_LIBS="
   MOOS
   MOOSGen
   anrp_util
   m
   pthread" \
simple_app_cmake_file

IVP_BUILD_BY_DEFAULT=ON \
IVP_INSTALL_COMPONENT=ivp-core-dev \
SUBDIR=lib_anrp_util         LIBNAME=anrp_util         simple_lib_cmake_file

SUBDIR=MOOSBlink \
PROGNAME=MOOSBlink \
IVP_BUILD_BY_DEFAULT=ON \
IVP_INSTALL_COMPONENT=ivp-tools \
LINK_LIBS="
   MOOS
   MOOSGen
   anrp_util
   m
   pthread" \
simple_app_cmake_file

SUBDIR=MOOSDump \
PROGNAME=MOOSDump \
IVP_BUILD_BY_DEFAULT=ON \
IVP_INSTALL_COMPONENT=ivp-core \
LINK_LIBS="
   MOOS
   MOOSGen
   mbutil
   m
   pthread" \
simple_app_cmake_file


SUBDIR=MOOSRemote \
PROGNAME=MOOSRemote \
IVP_BUILD_BY_DEFAULT=ON \
IVP_INSTALL_COMPONENT=ivp-core \
LINK_LIBS="
   MOOS
   MOOSGen
   mbutil
   curses
   m
   pthread" \
simple_app_cmake_file

SUBDIR=pCTDLogger \
PROGNAME=pCTDLogger \
IVP_BUILD_BY_DEFAULT=ON \
IVP_INSTALL_COMPONENT=ivp-tools \
LINK_LIBS="
   MOOS
   MOOSGen
   MOOSUtility
   anrp_util
   mbutil
   m
   pthread" \
simple_app_cmake_file

SUBDIR=pDynamicWaypointLL \
PROGNAME=pDynamicWaypointLL \
IVP_BUILD_BY_DEFAULT=ON \
IVP_INSTALL_COMPONENT=ivp-tools \
LINK_LIBS="
   MOOS
   MOOSGen
   MOOSUtility
   anrp_util
   m
   pthread" \
simple_app_cmake_file

SUBDIR=pGPSReTx \
PROGNAME=pGPSReTx \
IVP_BUILD_BY_DEFAULT=ON \
IVP_INSTALL_COMPONENT=ivp-tools \
LINK_LIBS="
   MOOS
   MOOSGen
   MOOSUtility
   anrp_util
   m
   pthread" \
simple_app_cmake_file

SUBDIR=pSamplingControl \
PROGNAME=pSamplingControl \
IVP_BUILD_BY_DEFAULT=ON \
IVP_INSTALL_COMPONENT=ivp-tools \
LINK_LIBS="
   MOOS
   MOOSGen
   MOOSUtility
   anrp_util
   m
   pthread" \
simple_app_cmake_file

SUBDIR=zlog2alog \
PROGNAME=zlog2alog \
IVP_BUILD_BY_DEFAULT=ON \
IVP_INSTALL_COMPONENT=ivp-tools \
LINK_LIBS="
   anrp_util
   m
   pthread
   z" \
simple_app_cmake_file

SUBDIR=zLoggerNG \
PROGNAME=zLoggerNG \
IVP_BUILD_BY_DEFAULT=ON \
IVP_INSTALL_COMPONENT=ivp-tools \
LINK_LIBS="
   MOOS
   MOOSGen
   anrp_util
   m
   pthread
   z" \
simple_app_cmake_file

SUBDIR=iGPS \
PROGNAME=iGPS \
IVP_BUILD_BY_DEFAULT=ON \
IVP_INSTALL_COMPONENT=none \
LINK_LIBS="
   MOOS
   MOOSGen
   MOOSUtility
   anrp_util
   m
   pthread" \
simple_app_cmake_file

SUBDIR=iPNICompass \
PROGNAME=iPNICompass \
IVP_BUILD_BY_DEFAULT=ON \
IVP_INSTALL_COMPONENT=ivp-core \
LINK_LIBS="
   MOOS
   MOOSGen
   MOOSUtility
   anrp_util
   m
   pthread" \
simple_app_cmake_file
