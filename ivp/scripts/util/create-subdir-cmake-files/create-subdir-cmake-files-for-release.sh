#!/bin/sh

set -e 

#===============================================================================

print_usage() {
   echo `basename ${0}` "<src-dir>"
   echo
   echo "Writes the CMakeLists.txt file for each subdirectory of <src-dir>."
   echo "This script uses built-in information about what subdirectories should "
   echo "exist, and how each subdirectory's CMakeLists.txt file should look."
}

#===============================================================================

parse_and_validate_cmd_line() {
   if [ "$#" -ne "1" ]; then
      print_usage
      exit 1
   fi 
   
   if [ -h $0 ]; then
      echo "You're running this script as a symbolic link.  Please don't: doing so "
      echo "makes it hard for this script to find some helper files that are supposed "
      echo "to reside in the same directory as the script itself."
      exit 1
   fi
   
   SCRIPT_DIR=`dirname ${0}`
   
   if [ ! -d "${1:?}" ]; then
      echo "<src-dir> doesn't seem to exist, or isn't a directory."
      exit 1
   fi
   
   SRC_DIR=${1}

   if [ ! -f ${SCRIPT_DIR}/simple-lib-template.txt ]; then
      echo "I should be able to find the file ./simple-lib-template.txt, "
      echo "but I can't."
      echo ""
      echo "You're supposed to run this script from it's own directory."
      exit 1
   fi

   if [ ! -f ${SCRIPT_DIR}/simple-app-template.txt ]; then
      echo "I should be able to find the file ./simple-app-template.txt, "
      echo "but I can't."
      echo ""
      echo "You're supposed to run this script from it's own directory."
      exit 1
   fi
}

#===============================================================================

lib_with_compile_flags_cmake_file() {
   if [ ! -d ${SRC_DIR}/${SUBDIR:?} ]; then
      echo "Something is wrong.  Directory ${SRC_DIR}/${SUBDIR:?} doesn't exist."
      exit 1
   fi

   if [ ! -f ${SCRIPT_DIR}/lib-with-compile-flags-template.txt ]; then
      echo "I should be able to find the file ./lib-with-compile-flags-template.txt, "
      echo "but I can't."
      echo ""
      echo "You're supposed to run this script from it's own directory."
      exit 1
   fi

   OUTPUT_FILE=${SRC_DIR}/${SUBDIR:?}/CMakeLists.txt

   echo "About to create file:" ${OUTPUT_FILE}

   cmake \
      -DINPUT_FILE=${SCRIPT_DIR}/lib-with-compile-flags-template.txt     \
      -DOUTPUT_FILE=${OUTPUT_FILE} \
      -DLIBNAME=${LIBNAME} \
      -DIVP_BUILD_BY_DEFAULT=${IVP_BUILD_BY_DEFAULT:?} \
      -DIVP_DATA_DIR='${IVP_DATA_DIR}' \
      -DCMAKE_BACKWARDS_COMPATIBILITY:STRING=2.4            \
      -P ${SCRIPT_DIR}/customize-template-file.cmake 
}

#===============================================================================

swig_lib_cmake_file() {
   if [ ! -d ${SRC_DIR}/${SUBDIR:?} ]; then
      echo "Something is wrong.  Directory ${SRC_DIR}/${SUBDIR:?} doesn't exist."
      exit 1
   fi

   if [ ! -f ${SCRIPT_DIR}/swig-lib-template.txt ]; then
      echo "I should be able to find the file ./lib-with-compile-flags-template.txt, "
      echo "but I can't."
      echo ""
      echo "You're supposed to run this script from it's own directory."
      exit 1
   fi

   OUTPUT_FILE=${SRC_DIR}/${SUBDIR:?}/CMakeLists.txt

   echo "About to create file:" ${OUTPUT_FILE}

   # Assume that the interface file name ends in ".i"
   cmake \
      -DINPUT_FILE=${SCRIPT_DIR}/swig-lib-template.txt     \
      -DOUTPUT_FILE=${OUTPUT_FILE} \
      -DLIBNAME=${LIBNAME} \
      -DINTERFACE_FILENAME=${INTERFACE_FILENAME:?} \
      -DIVP_BUILD_BY_DEFAULT=${IVP_BUILD_BY_DEFAULT:?} \
      -DIVP_INSTALL_COMPONENT=${IVP_INSTALL_COMPONENT:?} \
      -DIVP_SUBDIR_NAME=${SUBDIR:?} \
      -DCMAKE_BACKWARDS_COMPATIBILITY:STRING=2.4            \
      -P ${SCRIPT_DIR}/customize-template-file.cmake 
}

#===============================================================================

simple_lib_cmake_file() {
   if [ ! -d ${SRC_DIR}/${SUBDIR:?} ]; then
      echo "Something is wrong.  Directory ${SRC_DIR}/${SUBDIR:?} doesn't exist."
      exit 1
   fi

   OUTPUT_FILE=${SRC_DIR}/${SUBDIR:?}/CMakeLists.txt

   echo "About to create file:" ${OUTPUT_FILE}

   cmake \
      -DINPUT_FILE=${SCRIPT_DIR}/simple-lib-template.txt     \
      -DOUTPUT_FILE=${OUTPUT_FILE} \
      -DLIBNAME=${LIBNAME} \
      -DIVP_BUILD_BY_DEFAULT=${IVP_BUILD_BY_DEFAULT:?} \
      -DIVP_INSTALL_COMPONENT=${IVP_INSTALL_COMPONENT:?} \
      -DIVP_SUBDIR_NAME=${SUBDIR:?} \
      -DCMAKE_BACKWARDS_COMPATIBILITY:STRING=2.4            \
      -P ${SCRIPT_DIR}/customize-template-file.cmake 
}

#===============================================================================

simple_app_cmake_file() {
   if [ ! -d ${SRC_DIR}/${SUBDIR:?} ]; then
      echo "Something is wrong.  Directory ${SRC_DIR}/${SUBDIR:?} doesn't exist."
      exit 1
   fi

   OUTPUT_FILE=${SRC_DIR}/${SUBDIR:?}/CMakeLists.txt

   echo "About to create file:" ${OUTPUT_FILE}

   cmake \
      -DINPUT_FILE=${SCRIPT_DIR}/simple-app-template.txt     \
      -DOUTPUT_FILE=${OUTPUT_FILE} \
      -DPROGNAME=${PROGNAME:?} \
      -DIVP_BUILD_BY_DEFAULT=${IVP_BUILD_BY_DEFAULT:?} \
      -DIVP_INSTALL_COMPONENT=${IVP_INSTALL_COMPONENT:?} \
      -DIVP_SUBDIR_NAME=${SUBDIR:?} \
      -DLINK_LIBS="${LINK_LIBS:?}" \
      -DCMAKE_BACKWARDS_COMPATIBILITY:STRING=2.4            \
      -P ${SCRIPT_DIR}/customize-template-file.cmake 
}

#===============================================================================

fltk_app_cmake_file() {
   if [ ! -d ${SRC_DIR}/${SUBDIR:?} ]; then
      echo "Something is wrong.  Directory ${SRC_DIR}/${SUBDIR:?} doesn't exist."
      exit 1
   fi

   OUTPUT_FILE=${SRC_DIR}/${SUBDIR:?}/CMakeLists.txt

   echo "About to create file:" ${OUTPUT_FILE}

   cmake \
      -DINPUT_FILE=${SCRIPT_DIR}/fltk-app-template.txt     \
      -DOUTPUT_FILE=${OUTPUT_FILE} \
      -DPROGNAME=${PROGNAME:?} \
      -DIVP_BUILD_BY_DEFAULT=${IVP_BUILD_BY_DEFAULT:?} \
      -DLINK_LIBS="${LINK_LIBS:?}" \
      -DCMAKE_BACKWARDS_COMPATIBILITY:STRING=2.4            \
      -P ${SCRIPT_DIR}/customize-template-file.cmake 
}

#===============================================================================
# Main code...
#===============================================================================

parse_and_validate_cmd_line $*


#-------------------------------------------------------------------------------
# core libraries...
#-------------------------------------------------------------------------------

IVP_BUILD_BY_DEFAULT=ON \
IVP_INSTALL_COMPONENT=ivp-core-dev \
SUBDIR=lib_behaviors         LIBNAME=behaviors         simple_lib_cmake_file

IVP_BUILD_BY_DEFAULT=ON \
IVP_INSTALL_COMPONENT=ivp-core-dev \
SUBDIR=lib_behaviors-marine  LIBNAME=behaviors-marine  simple_lib_cmake_file

IVP_BUILD_BY_DEFAULT=ON \
IVP_INSTALL_COMPONENT=ivp-core-dev \
SUBDIR=lib_bhvutil           LIBNAME=bhvutil           simple_lib_cmake_file

IVP_BUILD_BY_DEFAULT=ON \
IVP_INSTALL_COMPONENT=ivp-core-dev \
SUBDIR=lib_genutil           LIBNAME=genutil           simple_lib_cmake_file

IVP_BUILD_BY_DEFAULT=ON \
IVP_INSTALL_COMPONENT=ivp-core-dev \
SUBDIR=lib_geometry          LIBNAME=geometry          simple_lib_cmake_file

IVP_BUILD_BY_DEFAULT=ON \
IVP_INSTALL_COMPONENT=ivp-core-dev \
SUBDIR=lib_helmivp           LIBNAME=helmivp           simple_lib_cmake_file

IVP_BUILD_BY_DEFAULT=ON \
IVP_INSTALL_COMPONENT=ivp-core-dev \
SUBDIR=lib_ivpbuild          LIBNAME=ivpbuild          simple_lib_cmake_file

IVP_BUILD_BY_DEFAULT=ON \
IVP_INSTALL_COMPONENT=ivp-core-dev \
SUBDIR=lib_ivpcore           LIBNAME=ivpcore           simple_lib_cmake_file

IVP_BUILD_BY_DEFAULT=ON \
IVP_INSTALL_COMPONENT=ivp-core-dev \
SUBDIR=lib_logic             LIBNAME=logic             simple_lib_cmake_file

IVP_BUILD_BY_DEFAULT=ON \
IVP_INSTALL_COMPONENT=ivp-core-dev \
SUBDIR=lib_mbutil            LIBNAME=mbutil            simple_lib_cmake_file

#-------------------------------------------------------------------------------
# tool libraries...
#-------------------------------------------------------------------------------

IVP_BUILD_BY_DEFAULT=ON \
IVP_INSTALL_COMPONENT=ivp-tools-dev \
SUBDIR=lib_navplot           LIBNAME=navplot           simple_lib_cmake_file


IVP_BUILD_BY_DEFAULT=ON \
IVP_INSTALL_COMPONENT=ivp-tools-dev \
IVP_DATA_DIR="${SRC_DIR}/../data/" \
SUBDIR=lib_marineview \
LIBNAME=marineview \
lib_with_compile_flags_cmake_file


#-------------------------------------------------------------------------------
# core apps...
#-------------------------------------------------------------------------------

SUBDIR=iMarineSim \
PROGNAME=iMarineSim \
IVP_BUILD_BY_DEFAULT=ON \
IVP_INSTALL_COMPONENT=ivp-core \
LINK_LIBS="
   MOOS
   MOOSGen
   mbutil
   geometry
   m
   pthread" \
simple_app_cmake_file

SUBDIR=pEchoVar \
PROGNAME=pEchoVar \
IVP_BUILD_BY_DEFAULT=ON \
IVP_INSTALL_COMPONENT=ivp-core \
LINK_LIBS="
   MOOS
   MOOSGen
   mbutil
   m
   pthread" \
simple_app_cmake_file


SUBDIR=pHelmIvP \
PROGNAME=pHelmIvP \
IVP_BUILD_BY_DEFAULT=ON \
IVP_INSTALL_COMPONENT=ivp-core \
LINK_LIBS="
   MOOS
   MOOSGen
   helmivp
   behaviors-marine
   bhvutil	
   behaviors
   mbutil 
   ivpbuild
   ivpcore 
   geometry
   logic
   m
   pthread" \
simple_app_cmake_file


SUBDIR=pMarinePID \
PROGNAME=pMarinePID \
IVP_BUILD_BY_DEFAULT=ON \
IVP_INSTALL_COMPONENT=ivp-core \
LINK_LIBS="
   MOOS
   MOOSGen
   mbutil
   geometry
   m
   pthread" \
simple_app_cmake_file


SUBDIR=pTransponderAIS \
PROGNAME=pTransponderAIS \
IVP_BUILD_BY_DEFAULT=ON \
IVP_INSTALL_COMPONENT=ivp-core \
LINK_LIBS="
   MOOS
   MOOSGen
   MOOSUtility
   mbutil
   m
   pthread" \
simple_app_cmake_file


SUBDIR=uProcessWatch \
PROGNAME=uProcessWatch \
IVP_BUILD_BY_DEFAULT=ON \
IVP_INSTALL_COMPONENT=ivp-core \
LINK_LIBS="
   MOOS
   MOOSGen
   mbutil
   m
   pthread" \
simple_app_cmake_file


SUBDIR=uTermCommand \
PROGNAME=uTermCommand \
IVP_BUILD_BY_DEFAULT=ON \
IVP_INSTALL_COMPONENT=ivp-core \
LINK_LIBS="
   MOOS
   MOOSGen
   MOOSUtility
   mbutil
   genutil
   m
   pthread" \
simple_app_cmake_file


SUBDIR=uTMS \
PROGNAME=uTMS \
IVP_BUILD_BY_DEFAULT=ON \
IVP_INSTALL_COMPONENT=ivp-core \
LINK_LIBS="
   MOOS
   MOOSGen
   genutil
   mbutil
   m
   pthread" \
simple_app_cmake_file


SUBDIR=uXMS \
PROGNAME=uXMS \
IVP_BUILD_BY_DEFAULT=ON \
IVP_INSTALL_COMPONENT=ivp-core \
LINK_LIBS="
   MOOS
   MOOSGen
   genutil
   mbutil
   m
   pthread" \
simple_app_cmake_file


#-------------------------------------------------------------------------------
# tools apps...
#-------------------------------------------------------------------------------

SUBDIR=app_logview \
PROGNAME=logview \
IVP_BUILD_BY_DEFAULT=ON \
IVP_INSTALL_COMPONENT=ivp-tools \
LINK_LIBS="
      navplot
      marineview
      geometry
      mbutil
      m" \
fltk_app_cmake_file


SUBDIR=app_polyview \
PROGNAME=polyview \
IVP_BUILD_BY_DEFAULT=ON \
IVP_INSTALL_COMPONENT=ivp-tools \
LINK_LIBS="
      marineview
      geometry
      mbutil
      m" \
fltk_app_cmake_file

SUBDIR=pMarineViewer \
PROGNAME=pMarineViewer \
IVP_BUILD_BY_DEFAULT=ON \
IVP_INSTALL_COMPONENT=ivp-tools \
LINK_LIBS="
      MOOS
      MOOSGen
      marineview
      mbutil
      genutil
      geometry
      m" \
fltk_app_cmake_file

#-------------------------------------------------------------------------------
