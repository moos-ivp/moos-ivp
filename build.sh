#!/bin/sh

# If anything goes wrong, stop right away.
set -e

cmake                            \
   -DFLTK_DIR=/usr/lib           \
   -DFLTK_INCLUDE_DIR=/usr/include \
   -DPYTHON_INCLUDE_PATH=/usr/include/python2.5 \
   -DBUILD_CONTROL_AND_NAV=ON    \
   -DBUILD_INSTRUMENTS=ON        \
   -DBUILD_TOOLS=ON              \
   -DBUILD_OCEAN_SIM=ON          \
   -DBUILD_MATLAB_TOOLS=OFF      \
   -DBUILD_LAND_INSTRUMENTS=ON   \
   -DBUILD_OCEAN_INSTRUMENTS=ON  \
   -DBUILD_UMVS=ON               \
   -DBUILD_UPB=ON                \
   ./

make
