#!/bin/sh

# Note by mikerb (Mar 8th 2008)
#
# The MOOS subdirectory is the exact directory distributed
# by Oxford. This script is used simply to facilitate the 
# switching off of modules that are not typically built in 
# local users of MOOS-IvP at MIT. 

cd MOOS
cmake                                           \
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
