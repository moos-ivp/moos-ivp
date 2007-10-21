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
IVP_INSTALL_COMPONENT=none \
SUBDIR=lib_abahr_util \
LIBNAME=abahr_util \
simple_lib_cmake_file

SUBDIR=uMicroModemAddPPS \
PROGNAME=uMicroModemAddPPS \
IVP_BUILD_BY_DEFAULT=ON \
IVP_INSTALL_COMPONENT=none \
LINK_LIBS="
   MOOS
   MOOSGen
   anrp_util
   abahr_util
   m
   pthread" \
simple_app_cmake_file

SUBDIR=uMicroModemWatchPPS \
PROGNAME=uMicroModemWatchPPS \
IVP_BUILD_BY_DEFAULT=ON \
IVP_INSTALL_COMPONENT=none \
LINK_LIBS="
   MOOS
   MOOSGen
   anrp_util
   abahr_util
   m
   pthread" \
simple_app_cmake_file

SUBDIR=pUDPBroadcastClient \
PROGNAME=pUDPBroadcastClient \
IVP_BUILD_BY_DEFAULT=OFF \
IVP_INSTALL_COMPONENT=none \
LINK_LIBS="
   MOOS
   MOOSGen
   opt
   newmat
   pthread" \
simple_app_cmake_file
