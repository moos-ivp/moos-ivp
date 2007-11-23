#!/bin/sh

set -e 

. ./util.sh

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
SUBDIR=lib_behaviors-colregs LIBNAME=behaviors-colregs simple_lib_cmake_file

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

IVP_BUILD_BY_DEFAULT=ON \
IVP_INSTALL_COMPONENT=ivp-core-dev \
SUBDIR=newmat10D           LIBNAME=newmat10D           simple_lib_cmake_file

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

IVP_BUILD_BY_DEFAULT=ON \
IVP_INSTALL_COMPONENT=ivp-tools-dev \
SUBDIR=lib_ipfview           LIBNAME=ipfview           simple_lib_cmake_file

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


SUBDIR=iMetaCompass \
PROGNAME=iMetaCompass \
IVP_BUILD_BY_DEFAULT=ON \
IVP_INSTALL_COMPONENT=ivp-core \
LINK_LIBS="
   MOOS
   MOOSGen
   mbutil
   m
   pthread" \
simple_app_cmake_file

SUBDIR=pDeployHandler \
PROGNAME=pDeployHandler \
IVP_BUILD_BY_DEFAULT=ON \
IVP_INSTALL_COMPONENT=ivp-core \
LINK_LIBS="
   MOOS
   MOOSGen
   mbutil
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
#    behaviors-sandbox
#    behaviors-don
   behaviors-colregs
#    behaviors-oxford
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
IVP_INSTALL_COMPONENT=ivp-core \
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


SUBDIR=pViewerRelay \
PROGNAME=pViewerRelay \
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

SUBDIR=pShipsideViewer \
PROGNAME=pShipsideViewer \
IVP_BUILD_BY_DEFAULT=ON \
IVP_INSTALL_COMPONENT=ivp-core \
LINK_LIBS="
      MOOS
      MOOSGen
      MOOSUtility
      marineview
      mbutil
      genutil
      geometry
      m" \
fltk_app_cmake_file

#-------------------------------------------------------------------------------
# tools apps...
#-------------------------------------------------------------------------------

SUBDIR=app_cpaview \
PROGNAME=cpaview \
IVP_BUILD_BY_DEFAULT=ON \
IVP_INSTALL_COMPONENT=ivp-tools \
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
IVP_INSTALL_COMPONENT=ivp-tools \
LINK_LIBS="
#       behaviors-sandbox
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

SUBDIR=app_glogview \
PROGNAME=glogview \
IVP_BUILD_BY_DEFAULT=ON \
IVP_INSTALL_COMPONENT=ivp-tools \
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
IVP_INSTALL_COMPONENT=ivp-tools \
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
IVP_INSTALL_COMPONENT=ivp-tools \
LINK_LIBS="
      ivpbuild
      mbutil
      ivpcore
      m" \
fltk_app_cmake_file

SUBDIR=app_logclip \
PROGNAME=logclip \
IVP_BUILD_BY_DEFAULT=ON \
IVP_INSTALL_COMPONENT=ivp-tools \
LINK_LIBS="
   mbutil
   m" \
simple_app_cmake_file

SUBDIR=app_logictest \
PROGNAME=ltest \
IVP_BUILD_BY_DEFAULT=ON \
IVP_INSTALL_COMPONENT=ivp-tools \
LINK_LIBS="
   logic
   mbutil
   m" \
simple_app_cmake_file

SUBDIR=app_logparse \
PROGNAME=logparse \
IVP_BUILD_BY_DEFAULT=ON \
IVP_INSTALL_COMPONENT=ivp-tools \
LINK_LIBS="
   mbutil
   m" \
simple_app_cmake_file

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

SUBDIR=app_peval \
PROGNAME=peval \
IVP_BUILD_BY_DEFAULT=ON \
IVP_INSTALL_COMPONENT=ivp-tools \
LINK_LIBS="
   navplot
   mbutil
   geometry
   m" \
simple_app_cmake_file

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

SUBDIR=app_quadview \
PROGNAME=quadview \
IVP_BUILD_BY_DEFAULT=ON \
IVP_INSTALL_COMPONENT=ivp-tools \
LINK_LIBS="
      marineview
      mbutil
      m" \
fltk_app_cmake_file

SUBDIR=app_splug \
PROGNAME=splug \
IVP_BUILD_BY_DEFAULT=ON \
IVP_INSTALL_COMPONENT=ivp-tools \
LINK_LIBS="
   mbutil
   m" \
simple_app_cmake_file

SUBDIR=app_test \
PROGNAME=stest \
IVP_BUILD_BY_DEFAULT=ON \
IVP_INSTALL_COMPONENT=ivp-tools \
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
IVP_INSTALL_COMPONENT=ivp-tools \
LINK_LIBS="
   mbutil
   ivpbuild
   ivpcore
   m" \
simple_app_cmake_file

SUBDIR=app_zaicview \
PROGNAME=zaicview \
IVP_BUILD_BY_DEFAULT=ON \
IVP_INSTALL_COMPONENT=ivp-tools \
LINK_LIBS="
   mbutil
   ivpbuild
   ivpcore
   m" \
fltk_app_cmake_file

SUBDIR=iHeadingMonitor \
PROGNAME=iHeadingMonitor \
IVP_BUILD_BY_DEFAULT=ON \
IVP_INSTALL_COMPONENT=ivp-tools \
LINK_LIBS="
   MOOS
   MOOSGen
   mbutil
   m
   pthread" \
simple_app_cmake_file

SUBDIR=iWifi \
PROGNAME=iWifi \
IVP_BUILD_BY_DEFAULT=ON \
IVP_INSTALL_COMPONENT=ivp-tools \
LINK_LIBS="
   MOOS
   MOOSGen
   m
   pthread" \
simple_app_cmake_file

SUBDIR=pACom \
PROGNAME=pACom \
IVP_BUILD_BY_DEFAULT=ON \
IVP_INSTALL_COMPONENT=ivp-tools \
LINK_LIBS="
   MOOS
   MOOSGen
   MOOSUtility
   m
   pthread" \
simple_app_cmake_file

SUBDIR=pFunctionSender \
PROGNAME=pFunctionSender \
IVP_BUILD_BY_DEFAULT=ON \
IVP_INSTALL_COMPONENT=ivp-tools \
LINK_LIBS="
   MOOS
   MOOSGen
   ivpbuild
   mbutil
   ivpcore
   m
   pthread" \
simple_app_cmake_file

SUBDIR=pMarineViewer \
PROGNAME=pMarineViewer \
IVP_BUILD_BY_DEFAULT=ON \
IVP_INSTALL_COMPONENT=ivp-tools \
LINK_LIBS="
      MOOS
      MOOSGen
      MOOSUtility
      marineview
      mbutil
      genutil
      geometry
      m" \
fltk_app_cmake_file

SUBDIR=pTrafficControl \
PROGNAME=pTrafficControl \
IVP_BUILD_BY_DEFAULT=OFF \
IVP_INSTALL_COMPONENT=ivp-tools \
LINK_LIBS="
   MOOS
   MOOSGen
   geometry
   mbutil
   m
   pthread" \
simple_app_cmake_file

SUBDIR=uFunctionViewer \
PROGNAME=uFunctionViewer \
IVP_BUILD_BY_DEFAULT=ON \
IVP_INSTALL_COMPONENT=ivp-tools \
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
IVP_INSTALL_COMPONENT=ivp-tools \
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

SUBDIR=app_gentraffic \
PROGNAME=gentraffic \
IVP_BUILD_BY_DEFAULT=ON \
IVP_INSTALL_COMPONENT=ivp-tools \
LINK_LIBS="
   MOOS
   MOOSGen
   geometry
   mbutil
   pthread" \
simple_app_cmake_file

SUBDIR=iMetaMemCompass \
PROGNAME=iMetaMemCompass \
IVP_BUILD_BY_DEFAULT=ON \
IVP_INSTALL_COMPONENT=ivp-tools \
LINK_LIBS="
   MOOS
   MOOSGen
   mbutil
   m
   pthread" \
simple_app_cmake_file

SUBDIR=iWebsite \
PROGNAME=iWebsite \
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

SUBDIR=pTransponderNMEA \
PROGNAME=pTransponderNMEA \
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

SUBDIR=pVirtualTether \
PROGNAME=pVirtualTether \
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

SUBDIR=uRepeater \
PROGNAME=uRepeater \
IVP_BUILD_BY_DEFAULT=ON \
IVP_INSTALL_COMPONENT=ivp-tools \
LINK_LIBS="
   MOOS
   MOOSGen
   mbutil
   m
   pthread" \
simple_app_cmake_file
