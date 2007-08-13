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

IVP_BUILD_BY_DEFAULT=ON \
SUBDIR=lib_anrp_util         LIBNAME=anrp_util         simple_lib_cmake_file

IVP_BUILD_BY_DEFAULT=ON \
SUBDIR=lib_artifacts         LIBNAME=artifacts         simple_lib_cmake_file

IVP_BUILD_BY_DEFAULT=ON \
SUBDIR=lib_behaviors         LIBNAME=behaviors         simple_lib_cmake_file

IVP_BUILD_BY_DEFAULT=ON \
SUBDIR=lib_behaviors-attic   LIBNAME=behaviors-attic   simple_lib_cmake_file

IVP_BUILD_BY_DEFAULT=ON \
SUBDIR=lib_behaviors-colregs LIBNAME=behaviors-colregs simple_lib_cmake_file

IVP_BUILD_BY_DEFAULT=ON \
SUBDIR=lib_behaviors-don     LIBNAME=behaviors-don     simple_lib_cmake_file

IVP_BUILD_BY_DEFAULT=ON \
SUBDIR=lib_behaviors-marine  LIBNAME=behaviors-marine  simple_lib_cmake_file

IVP_BUILD_BY_DEFAULT=ON \
SUBDIR=lib_behaviors-oxford  LIBNAME=behaviors-oxford  simple_lib_cmake_file

IVP_BUILD_BY_DEFAULT=ON \
SUBDIR=lib_behaviors-sandbox LIBNAME=behaviors-sandbox simple_lib_cmake_file

IVP_BUILD_BY_DEFAULT=ON \
SUBDIR=lib_bhvutil           LIBNAME=bhvutil           simple_lib_cmake_file

IVP_BUILD_BY_DEFAULT=ON \
SUBDIR=lib_genutil           LIBNAME=genutil           simple_lib_cmake_file

IVP_BUILD_BY_DEFAULT=ON \
SUBDIR=lib_geometry          LIBNAME=geometry          simple_lib_cmake_file

IVP_BUILD_BY_DEFAULT=ON \
SUBDIR=lib_helmivp           LIBNAME=helmivp           simple_lib_cmake_file

IVP_BUILD_BY_DEFAULT=ON \
SUBDIR=lib_ipfview           LIBNAME=ipfview           simple_lib_cmake_file

IVP_BUILD_BY_DEFAULT=ON \
SUBDIR=lib_ivpbuild          LIBNAME=ivpbuild          simple_lib_cmake_file

IVP_BUILD_BY_DEFAULT=ON \
SUBDIR=lib_ivpbuild-extra    LIBNAME=ivpbuild-extra    simple_lib_cmake_file

IVP_BUILD_BY_DEFAULT=ON \
SUBDIR=lib_ivpcore           LIBNAME=ivpcore           simple_lib_cmake_file

IVP_BUILD_BY_DEFAULT=ON \
SUBDIR=lib_logic             LIBNAME=logic             simple_lib_cmake_file

IVP_BUILD_BY_DEFAULT=ON \
SUBDIR=lib_mbutil            LIBNAME=mbutil            simple_lib_cmake_file

IVP_BUILD_BY_DEFAULT=ON \
SUBDIR=lib_mbutiltest        LIBNAME=mbutiltest        simple_lib_cmake_file

IVP_BUILD_BY_DEFAULT=ON \
SUBDIR=lib_navplot           LIBNAME=navplot           simple_lib_cmake_file

IVP_BUILD_BY_DEFAULT=ON \
SUBDIR=lib_track             LIBNAME=opt               simple_lib_cmake_file

IVP_BUILD_BY_DEFAULT=ON \
IVP_DATA_DIR="${SRC_DIR}/../data/" \
SUBDIR=lib_marineview \
LIBNAME=marineview \
lib_with_compile_flags_cmake_file

SUBDIR=app_artfieldgenerator \
PROGNAME=artfieldgenerator \
IVP_BUILD_BY_DEFAULT=ON \
LINK_LIBS="MOOS MOOSGen mbutil" \
simple_app_cmake_file

SUBDIR=app_cpaview \
PROGNAME=cpaview \
IVP_BUILD_BY_DEFAULT=ON \
LINK_LIBS="
      marineview
      ipfview
      geometry
      ivpbuild
      mbutil
      ivpcore
      m" \
fltk_app_cmake_file

SUBDIR=app_ffview \
PROGNAME=ffview \
IVP_BUILD_BY_DEFAULT=ON \
LINK_LIBS="
      behaviors-don
      behaviors-sandbox
      behaviors-marine
      bhvutil
      ipfview
      behaviors
      geometry
      ivpbuild
      ivpcore
      mbutil
      m" \
fltk_app_cmake_file

SUBDIR=app_fview \
PROGNAME=fview \
IVP_BUILD_BY_DEFAULT=ON \
LINK_LIBS="
      behaviors-don
      behaviors-sandbox
      behaviors-marine
      bhvutil
      ipfview
      behaviors
      geometry
      ivpbuild
      ivpcore
      m
      mbutil" \
fltk_app_cmake_file

SUBDIR=app_glogview \
PROGNAME=glogview \
IVP_BUILD_BY_DEFAULT=ON \
LINK_LIBS="
      navplot
      ipfview
      marineview
      geometry
      mbutil
      ivpbuild
      ivpcore
      m" \
fltk_app_cmake_file

SUBDIR=app_gridgen \
PROGNAME=gridgen \
IVP_BUILD_BY_DEFAULT=ON \
LINK_LIBS="
   ivpbuild
   ivpcore
   geometry
   mbutil
   m" \
simple_app_cmake_file

SUBDIR=app_gzaicview \
PROGNAME=gzaicview \
IVP_BUILD_BY_DEFAULT=ON \
LINK_LIBS="
      ivpbuild
      mbutil
      ivpcore
      m" \
fltk_app_cmake_file

SUBDIR=app_hexaview \
PROGNAME=hexaview \
IVP_BUILD_BY_DEFAULT=ON \
LINK_LIBS="
      marineview
      geometry
      mbutil
      m" \
fltk_app_cmake_file

SUBDIR=app_logclip \
PROGNAME=logclip \
IVP_BUILD_BY_DEFAULT=ON \
LINK_LIBS="
   mbutil
   m" \
simple_app_cmake_file

SUBDIR=app_logictest \
PROGNAME=ltest \
IVP_BUILD_BY_DEFAULT=ON \
LINK_LIBS="
   logic
   mbutil
   m" \
simple_app_cmake_file

SUBDIR=app_logparse \
PROGNAME=logparse \
IVP_BUILD_BY_DEFAULT=ON \
LINK_LIBS="
   mbutil
   m" \
simple_app_cmake_file

SUBDIR=app_logview \
PROGNAME=logview \
IVP_BUILD_BY_DEFAULT=ON \
LINK_LIBS="
      navplot
      marineview
      geometry
      mbutil
      m" \
fltk_app_cmake_file

SUBDIR=app_peval \
PROGNAME=peval \
IVP_BUILD_BY_DEFAULT=ON \
LINK_LIBS="
   navplot
   mbutil
   geometry
   m" \
simple_app_cmake_file

SUBDIR=app_polyview \
PROGNAME=polyview \
IVP_BUILD_BY_DEFAULT=ON \
LINK_LIBS="
      marineview
      geometry
      mbutil
      m" \
fltk_app_cmake_file

SUBDIR=app_quadview \
PROGNAME=quadview \
IVP_BUILD_BY_DEFAULT=ON \
LINK_LIBS="
      marineview
      mbutil
      m" \
fltk_app_cmake_file

SUBDIR=app_splug \
PROGNAME=splug \
IVP_BUILD_BY_DEFAULT=ON \
LINK_LIBS="
   mbutil
   m" \
simple_app_cmake_file

SUBDIR=app_test \
PROGNAME=stest \
IVP_BUILD_BY_DEFAULT=ON \
LINK_LIBS="
   ivpbuild
   mbutil
   geometry
   ivpcore
   logic
   m" \
simple_app_cmake_file

SUBDIR=app_tupelize \
PROGNAME=tupelize \
IVP_BUILD_BY_DEFAULT=ON \
LINK_LIBS="
   mbutil
   ivpbuild
   ivpcore
   m" \
simple_app_cmake_file

SUBDIR=app_zaicview \
PROGNAME=zaicview \
IVP_BUILD_BY_DEFAULT=ON \
LINK_LIBS="
   mbutil
   ivpbuild
   ivpcore
   m" \
fltk_app_cmake_file

SUBDIR=iHeadingMonitor \
PROGNAME=iHeadingMonitor \
IVP_BUILD_BY_DEFAULT=ON \
LINK_LIBS="
   MOOS
   MOOSGen
   mbutil
   m
   pthread" \
simple_app_cmake_file

SUBDIR=iMarineSim \
PROGNAME=iMarineSim \
IVP_BUILD_BY_DEFAULT=ON \
LINK_LIBS="
   MOOS
   MOOSGen
   mbutil
   geometry
   m
   pthread" \
simple_app_cmake_file

SUBDIR=iMetaCompass \
PROGNAME=iMetaCompass \
IVP_BUILD_BY_DEFAULT=ON \
LINK_LIBS="
   MOOS
   MOOSGen
   mbutil
   m
   pthread" \
simple_app_cmake_file

SUBDIR=iMicroModem \
PROGNAME=iMicroModem \
IVP_BUILD_BY_DEFAULT=ON \
LINK_LIBS="
   MOOS 
   MOOSGen
   pthread" \
simple_app_cmake_file

SUBDIR=iPNICompass \
PROGNAME=iPNICompass \
IVP_BUILD_BY_DEFAULT=ON \
LINK_LIBS="
   MOOS
   MOOSGen
   MOOSUtility
   anrp_util
   m
   pthread" \
simple_app_cmake_file

SUBDIR=iPWMController \
PROGNAME=iPWMController \
IVP_BUILD_BY_DEFAULT=ON \
LINK_LIBS="
   MOOS
   MOOSGen
   anrp_util
   m
   pthread" \
simple_app_cmake_file

SUBDIR=iWifi \
PROGNAME=iWifi \
IVP_BUILD_BY_DEFAULT=ON \
LINK_LIBS="
   MOOS
   MOOSGen
   m
   pthread" \
simple_app_cmake_file

SUBDIR=p1BTracker \
PROGNAME=p1BTracker \
IVP_BUILD_BY_DEFAULT=ON \
LINK_LIBS="
   MOOS
   MOOSGen
   opt
   m
   pthread" \
simple_app_cmake_file

SUBDIR=p2BTracker \
PROGNAME=p2BTracker \
IVP_BUILD_BY_DEFAULT=ON \
LINK_LIBS="
   MOOS
   MOOSGen
   opt
   m
   pthread" \
simple_app_cmake_file

SUBDIR=pACom \
PROGNAME=pACom \
IVP_BUILD_BY_DEFAULT=ON \
LINK_LIBS="
   MOOS
   MOOSGen
   MOOSUtility
   m
   pthread" \
simple_app_cmake_file

SUBDIR=pArtifactSensorSim \
PROGNAME=pArtifactSensorSim \
IVP_BUILD_BY_DEFAULT=OFF \
LINK_LIBS="
   MOOS
   MOOSGen
   mbutil
   m
   pthread" \
simple_app_cmake_file

SUBDIR=pBearings \
PROGNAME=pBearings \
IVP_BUILD_BY_DEFAULT=ON \
LINK_LIBS="
   MOOS
   MOOSGen
   opt
   newmat
   m
   pthread" \
simple_app_cmake_file

SUBDIR=pBearingsSim \
PROGNAME=pBearingsSim \
IVP_BUILD_BY_DEFAULT=ON \
LINK_LIBS="
   MOOS
   MOOSGen
   opt
   m
   pthread" \
simple_app_cmake_file

SUBDIR=pClusterManager \
PROGNAME=pClusterManager \
IVP_BUILD_BY_DEFAULT=ON \
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
LINK_LIBS="
   MOOS
   MOOSGen
   mbutil
   m
   pthread" \
simple_app_cmake_file

SUBDIR=pExtTracker \
PROGNAME=pExtTracker \
IVP_BUILD_BY_DEFAULT=ON \
LINK_LIBS="
   MOOS
   MOOSGen
   m
   pthread" \
simple_app_cmake_file

SUBDIR=pFunctionSender \
PROGNAME=pFunctionSender \
IVP_BUILD_BY_DEFAULT=ON \
LINK_LIBS="
   MOOS
   MOOSGen
   ivpbuild
   mbutil
   ivpcore
   m
   pthread" \
simple_app_cmake_file

SUBDIR=pHelmIvP \
PROGNAME=pHelmIvP \
IVP_BUILD_BY_DEFAULT=ON \
LINK_LIBS="
   MOOS
   MOOSGen
   helmivp
   behaviors-marine
   behaviors-sandbox
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

SUBDIR=pLoiterControl \
PROGNAME=pLoiterControl \
IVP_BUILD_BY_DEFAULT=ON \
LINK_LIBS="
   MOOS
   MOOSGen
   mbutil
   geometry
   m
   pthread" \
simple_app_cmake_file

SUBDIR=pMarinePID \
PROGNAME=pMarinePID \
IVP_BUILD_BY_DEFAULT=ON \
LINK_LIBS="
   MOOS
   MOOSGen
   mbutil
   geometry
   m
   pthread" \
simple_app_cmake_file

SUBDIR=pMarineViewer \
PROGNAME=pMarineViewer \
IVP_BUILD_BY_DEFAULT=ON \
LINK_LIBS="
      MOOS
      MOOSGen
      marineview
      mbutil
      genutil
      geometry
      m" \
fltk_app_cmake_file

SUBDIR=pMineSim \
PROGNAME=pMineSim \
IVP_BUILD_BY_DEFAULT=ON \
LINK_LIBS="
   MOOS
   MOOSGen
   geometry
   mbutil
   m
   pthread" \
simple_app_cmake_file

SUBDIR=pTarget \
PROGNAME=pTarget \
IVP_BUILD_BY_DEFAULT=ON \
LINK_LIBS="
   MOOS
   MOOSGen
   geometry
   mbutil
   m
   pthread" \
simple_app_cmake_file

SUBDIR=pTrafficControl \
PROGNAME=pTrafficControl \
IVP_BUILD_BY_DEFAULT=OFF \
LINK_LIBS="
   MOOS
   MOOSGen
   geometry
   mbutil
   m
   pthread" \
simple_app_cmake_file

SUBDIR=pTransponderAIS \
PROGNAME=pTransponderAIS \
IVP_BUILD_BY_DEFAULT=ON \
LINK_LIBS="
   MOOS
   MOOSGen
   mbutil
   m
   pthread" \
simple_app_cmake_file

SUBDIR=pWatchDog \
PROGNAME=pWatchDog \
IVP_BUILD_BY_DEFAULT=OFF \
LINK_LIBS="
   MOOS
   MOOSGen
   mbutil
   m
   pthread" \
simple_app_cmake_file

SUBDIR=uFunctionViewer \
PROGNAME=uFunctionViewer \
IVP_BUILD_BY_DEFAULT=ON \
LINK_LIBS="
      MOOS
      MOOSGen
      geometry
      genutil
      mbutil
      pthread
      ivpbuild
      ivpcore
      m" \
fltk_app_cmake_file

SUBDIR=uFunctionVis \
PROGNAME=uFunctionVis \
IVP_BUILD_BY_DEFAULT=ON \
LINK_LIBS="
      MOOS
      MOOSGen
      ipfview
      geometry
      genutil
      mbutil
      ivpbuild
      ivpcore
      m" \
fltk_app_cmake_file

SUBDIR=uMOOSPoke \
PROGNAME=uMOOSPoke \
IVP_BUILD_BY_DEFAULT=OFF \
LINK_LIBS="
   MOOS
   MOOSGen
   m
   pthread" \
simple_app_cmake_file

SUBDIR=uProcessWatch \
PROGNAME=uProcessWatch \
IVP_BUILD_BY_DEFAULT=ON \
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
LINK_LIBS="
   MOOS
   MOOSGen
   genutil
   mbutil
   m
   pthread" \
simple_app_cmake_file


SUBDIR=uTMS \
PROGNAME=uTMS \
IVP_BUILD_BY_DEFAULT=OFF \
INTERFACE_FILENAME=MOOS.i \
LINK_LIBS="
   MOOS
   MOOSGen
   genutil
   mbutil
   m
   pthread" \

IVP_BUILD_BY_DEFAULT=OFF \
SUBDIR=uVoice \
LIBNAME=_MOOS \
INTERFACE_FILENAME=MOOS.i \
swig_lib_cmake_file

SUBDIR=uXMS \
PROGNAME=uXMS \
IVP_BUILD_BY_DEFAULT=ON \
LINK_LIBS="
   MOOS
   MOOSGen
   genutil
   mbutil
   m
   pthread" \
simple_app_cmake_file
