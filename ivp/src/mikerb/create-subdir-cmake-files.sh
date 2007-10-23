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
SUBDIR=lib_behaviors-attic   LIBNAME=behaviors-attic   simple_lib_cmake_file

# Temporarily disabled because of this build error:
# [ 75%] Building CXX object ivp/src/lib_helmivp/CMakeFiles/helmivp.dir/Populator_BehaviorSet.o
# /home/cjc/trunk/ivp/src/mikerb/lib_behaviors-oxford/SimpleBehaviour.h: In member function ‘virtual IvPFunction* CSimpleBehavior::produceOF()’:
# /home/cjc/trunk/ivp/src/mikerb/lib_behaviors-oxford/SimpleBehaviour.h:114: error: ‘class OF_Reflector’ has no member named ‘create_uniform’
# make[2]: *** [ivp/src/lib_helmivp/CMakeFiles/helmivp.dir/Populator_BehaviorSet.o] Error 1
# make[1]: *** [ivp/src/lib_helmivp/CMakeFiles/helmivp.dir/all] Error 2
IVP_BUILD_BY_DEFAULT=OFF \
IVP_INSTALL_COMPONENT=none \
SUBDIR=lib_behaviors-oxford  LIBNAME=behaviors-oxford  simple_lib_cmake_file

IVP_BUILD_BY_DEFAULT=ON \
IVP_INSTALL_COMPONENT=none \
SUBDIR=lib_behaviors-sandbox LIBNAME=behaviors-sandbox simple_lib_cmake_file

IVP_BUILD_BY_DEFAULT=ON \
IVP_INSTALL_COMPONENT=none \
SUBDIR=lib_ivpbuild-extra    LIBNAME=ivpbuild-extra    simple_lib_cmake_file

IVP_BUILD_BY_DEFAULT=ON \
IVP_INSTALL_COMPONENT=none \
SUBDIR=lib_mbutiltest        LIBNAME=mbutiltest        simple_lib_cmake_file

SUBDIR=pClusterManager \
PROGNAME=pClusterManager \
IVP_BUILD_BY_DEFAULT=ON \
IVP_INSTALL_COMPONENT=none \
LINK_LIBS="
   MOOS
   MOOSGen
   mbutil
   geometry
   m
   pthread" \
simple_app_cmake_file

SUBDIR=pWatchDog \
PROGNAME=pWatchDog \
IVP_BUILD_BY_DEFAULT=ON \
IVP_INSTALL_COMPONENT=none \
LINK_LIBS="
   MOOS
   MOOSGen
   mbutil
   m
   pthread" \
simple_app_cmake_file

IVP_BUILD_BY_DEFAULT=OFF \
IVP_INSTALL_COMPONENT=none \
SUBDIR=uVoice \
LIBNAME=_MOOS \
INTERFACE_FILENAME=MOOS.i \
swig_lib_cmake_file
