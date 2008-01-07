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

SUBDIR=iSerialNMEA \
PROGNAME=iSerialNMEA \
IVP_BUILD_BY_DEFAULT=ON \
IVP_INSTALL_COMPONENT=none \
LINK_LIBS="
   MOOS
   MOOSGen
   MOOSUtility
   mbutil
   m
   pthread" \
simple_app_cmake_file

SUBDIR=iWebsite \
PROGNAME=iWebsite \
IVP_BUILD_BY_DEFAULT=ON \
IVP_INSTALL_COMPONENT=none \
LINK_LIBS="
   MOOS
   MOOSGen
   MOOSUtility
   mbutil
   m
   pthread" \
simple_app_cmake_file


# This app also needs an INCLUDE_DIRECTORIES(/usr/include/mysql) command, which 
# this generation script doesn't currently support afaik.  So for now, don't
# automate the creation of this CMakeLists.txt file.  (This is easy to fix, btw.)
#
# SUBDIR=pAIS2SQL \
# PROGNAME=pAIS2SQL \
# IVP_BUILD_BY_DEFAULT=ON \
# IVP_INSTALL_COMPONENT=none \
# LINK_LIBS="
#    mysqlclient
#    MOOS
#    MOOSGen
#    mbutil
#    m
#    pthread" \
# simple_app_cmake_file

SUBDIR=pCTDLogger \
PROGNAME=pCTDLogger \
IVP_BUILD_BY_DEFAULT=ON \
IVP_INSTALL_COMPONENT=none \
LINK_LIBS="
   MOOS
   MOOSGen
   MOOSUtility
   mbutil
   m
   pthread" \
simple_app_cmake_file

SUBDIR=pSoundSpeed \
PROGNAME=pSoundSpeed \
IVP_BUILD_BY_DEFAULT=ON \
IVP_INSTALL_COMPONENT=none \
LINK_LIBS="
   MOOS
   MOOSGen
   MOOSUtility
   mbutil
   m
   pthread" \
simple_app_cmake_file


SUBDIR=pVirtualTether \
PROGNAME=pVirtualTether \
IVP_BUILD_BY_DEFAULT=ON \
IVP_INSTALL_COMPONENT=none \
LINK_LIBS="
   MOOS
   MOOSGen
   MOOSUtility
   mbutil
   m
   pthread" \
simple_app_cmake_file


SUBDIR=zlog2alog_tes \
PROGNAME=zlog2alog_tes \
IVP_BUILD_BY_DEFAULT=ON \
IVP_INSTALL_COMPONENT=none \
LINK_LIBS="
   anrp_util
   z
   m
   pthread" \
simple_app_cmake_file


SUBDIR=addlatlong \
PROGNAME=addlatlong \
IVP_BUILD_BY_DEFAULT=OFF \
IVP_INSTALL_COMPONENT=none \
LINK_LIBS="
   MOOS
   MOOSGen
   MOOSUtility
   mbutil
   pthread" \
simple_app_cmake_file


SUBDIR=alog_to_btr \
PROGNAME=alog_to_btr \
IVP_BUILD_BY_DEFAULT=OFF \
IVP_INSTALL_COMPONENT=none \
LINK_LIBS="
   " \
simple_app_cmake_file


SUBDIR=btr_fix \
PROGNAME=btr_fix \
IVP_BUILD_BY_DEFAULT=OFF \
IVP_INSTALL_COMPONENT=none \
LINK_LIBS="
   " \
simple_app_cmake_file

SUBDIR=time_fix_alog \
PROGNAME=time_fix_alog \
IVP_BUILD_BY_DEFAULT=ON \
IVP_INSTALL_COMPONENT=none \
LINK_LIBS="
   m" \
simple_app_cmake_file


SUBDIR=modem_csv \
PROGNAME=modem_csv \
IVP_BUILD_BY_DEFAULT=OFF \
IVP_INSTALL_COMPONENT=none \
LINK_LIBS="
   " \
simple_app_cmake_file

