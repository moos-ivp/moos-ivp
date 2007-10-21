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

IVP_BUILD_BY_DEFAULT=ON \
IVP_INSTALL_COMPONENT=ivp-core-dev \
SUBDIR=lib_behaviors-don     LIBNAME=behaviors-don     simple_lib_cmake_file

IVP_BUILD_BY_DEFAULT=ON \
IVP_INSTALL_COMPONENT=ivp-tools-dev \
SUBDIR=lib_track             LIBNAME=opt               simple_lib_cmake_file

SUBDIR=p1BTracker \
PROGNAME=p1BTracker \
IVP_BUILD_BY_DEFAULT=ON \
IVP_INSTALL_COMPONENT=ivp-tools \
LINK_LIBS="
   MOOS
   MOOSGen
   opt
   m
   pthread" \
simple_app_cmake_file

SUBDIR=pBearings \
PROGNAME=pBearings \
IVP_BUILD_BY_DEFAULT=ON \
IVP_INSTALL_COMPONENT=ivp-tools \
LINK_LIBS="
   MOOS
   MOOSGen
   opt
   newmat10D
   m
   pthread" \
simple_app_cmake_file

SUBDIR=pBearingsSim \
PROGNAME=pBearingsSim \
IVP_BUILD_BY_DEFAULT=ON \
IVP_INSTALL_COMPONENT=ivp-tools \
LINK_LIBS="
   MOOS
   MOOSGen
   opt
   m
   pthread" \
simple_app_cmake_file

SUBDIR=pExtTracker \
PROGNAME=pExtTracker \
IVP_BUILD_BY_DEFAULT=ON \
IVP_INSTALL_COMPONENT=ivp-tools \
LINK_LIBS="
   MOOS
   MOOSGen
   m
   pthread" \
simple_app_cmake_file

SUBDIR=pScheduler \
PROGNAME=pScheduler \
IVP_BUILD_BY_DEFAULT=ON \
IVP_INSTALL_COMPONENT=ivp-core \
LINK_LIBS="
   MOOS
   MOOSGen
   genutil
   mbutil
   pthread" \
simple_app_cmake_file

SUBDIR=pSensorSim \
PROGNAME=pSensorSim \
IVP_BUILD_BY_DEFAULT=ON \
IVP_INSTALL_COMPONENT=ivp-tools \
LINK_LIBS="
   MOOS
   MOOSGen
   mbutil
   m
   pthread
   artifacts
   geometry" \
simple_app_cmake_file

SUBDIR=pTarget \
PROGNAME=pTarget \
IVP_BUILD_BY_DEFAULT=ON \
IVP_INSTALL_COMPONENT=ivp-tools \
LINK_LIBS="
   MOOS
   MOOSGen
   geometry
   mbutil
   m
   pthread" \
simple_app_cmake_file
