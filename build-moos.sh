#!/bin/sh

# Note by mikerb (Mar 8th 2008)
#
# The MOOS subdirectory is the exact directory distributed
# by Oxford, with the following two exceptions. Two local
# edits were made:
#
# (1) A MOOS-VERSION-IS-V-X file is added to MOOS/ to show
#     which Oxford MOOS version is in this MOOS-IvP tree.
# (2) MOOS/NavigationAndControl/CMakeLists.txt is edited to 
#     not build pNav, pHelm, MOOSNavLib, MOOSTaskLib, four
#     fairly large modules generally not needed by users of
#     this repository. There was no CMake BUILD flag for
#     shutting these off, other than shutting off all of
#     NavigationAndControl which contains iRemote.

MOOS_PACKAGE_VERSION=7.0.1
set -e

cd MOOS
cmake                                           \
   -DMOOS_VERSION=${MOOS_PACKAGE_VERSION:?}     \
   -DFLTK_DIR=/usr/lib                          \
   -DFLTK_INCLUDE_DIR=/usr/include              \
   -DPYTHON_INCLUDE_PATH=/usr/include/python2.5 \
   -DBUILD_CONTROL_AND_NAV=ON                   \
   -DBUILD_INSTRUMENTS=ON                      \
   -DDB_HAS_HTTP=ON                             \
   -DBUILD_TOOLS=ON                             \
   -DBUILD_OCEAN_SIM=ON                         \
   -DBUILD_NAVIGATION=OFF                       \
   -DBUILD_HELM=OFF                             \
   -DBUILD_NEWMAT=OFF                           \
   -DBUILD_GRAPHICAL_TOOLS=OFF                   \
   -DBUILD_MATLAB_TOOLS=OFF                     \
   -DMATLAB_DIR=/usr/local/matlab-2007a         \
   -DBUILD_LAND_INSTRUMENTS=OFF                 \
   -DBUILD_OCEAN_INSTRUMENTS=OFF                \
   -DBUILD_UMVS=ON                              \
   -DBUILD_UPB=ON                               \
   ./

make
