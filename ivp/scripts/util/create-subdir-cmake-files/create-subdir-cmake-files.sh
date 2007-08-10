#!/bin/sh

set -e 
#===============================================================================

simple_lib_cmake_file() {
   if [ ! -d ${SRC_DIR}/${SUBDIR:?} ]; then
      echo "Something is wrong.  Directory ${SRC_DIR}/${SUBDIR:?} doesn't exist."
      exit 1
   fi

   if [ ! -f ${SCRIPT_DIR}/simple-lib-template.txt ]; then
      echo "I should be able to find the file ./simple-lib-template.txt, "
      echo "but I can't."
      echo ""
      echo "You're supposed to run this script from it's own directory."
      exit 1
   fi

   cmake \
      -DINPUT_FILE=${SCRIPT_DIR}/simple-lib-template.txt     \
      -DOUTPUT_FILE=${SUBDIR:?}/CMakeLists.txt \
      -DLIBNAME=${LIBNAME} \
      -DCMAKE_BACKWARDS_COMPATIBILITY:STRING=2.4            \
      -P ${SCRIPT_DIR}/customize-template-file.cmake 
}

#===============================================================================

# p   rint_usage_and_exit() {
# }

#===============================================================================

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


SUBDIR=lib_anrp_util         LIBNAME=anrp_util         simple_lib_cmake_file
SUBDIR=lib_artifacts         LIBNAME=artifacts         simple_lib_cmake_file
SUBDIR=lib_behaviors         LIBNAME=behaviors         simple_lib_cmake_file
SUBDIR=lib_behaviors-attic   LIBNAME=behaviors-attic   simple_lib_cmake_file
SUBDIR=lib_behaviors-colregs LIBNAME=behaviors-colregs simple_lib_cmake_file
SUBDIR=lib_behaviors-don     LIBNAME=behaviors-don     simple_lib_cmake_file
SUBDIR=lib_behaviors-marine  LIBNAME=behaviors-marine  simple_lib_cmake_file
SUBDIR=lib_behaviors-oxford  LIBNAME=behaviors-oxford  simple_lib_cmake_file
SUBDIR=lib_behaviors-sandbox LIBNAME=behaviors-sandbox simple_lib_cmake_file
SUBDIR=lib_bhvutil           LIBNAME=bhvutil           simple_lib_cmake_file
SUBDIR=lib_genutil           LIBNAME=genutil           simple_lib_cmake_file
SUBDIR=lib_geometry          LIBNAME=geometry          simple_lib_cmake_file
SUBDIR=lib_helmivp           LIBNAME=helmivp           simple_lib_cmake_file
SUBDIR=lib_ipfview           LIBNAME=ipfview           simple_lib_cmake_file
SUBDIR=lib_ivpbuild          LIBNAME=ivpbuild          simple_lib_cmake_file
SUBDIR=lib_ivpbuild-extra    LIBNAME=ivpbuild-extra    simple_lib_cmake_file
SUBDIR=lib_ivpcore           LIBNAME=ivpcore           simple_lib_cmake_file
SUBDIR=lib_logic             LIBNAME=logic             simple_lib_cmake_file
SUBDIR=lib_marineview        LIBNAME=marineview        simple_lib_cmake_file
SUBDIR=lib_mbutil            LIBNAME=mbutil            simple_lib_cmake_file
SUBDIR=lib_mbutiltest        LIBNAME=mbutiltest        simple_lib_cmake_file
SUBDIR=lib_navplot           LIBNAME=navplot           simple_lib_cmake_file
SUBDIR=lib_track             LIBNAME=opt               simple_lib_cmake_file
