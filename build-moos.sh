#!/bin/bash

INVOC_ABS_DIR="`pwd`"
MOOS_ABS_DIR="${INVOC_ABS_DIR}/MOOS"

echo "  INVOC_ABS_DIR: " ${INVOC_ABS_DIR}
echo "  MOOS_ABS_DIR:  " ${MOOS_ABS_DIR}

# Setup C and C++ Compiler flags for Mac and Linux. 
if [ "`uname`" == "Darwin" ] ; then
  echo "Building MOOS for Apple"
else
  echo "Building MOOS for Linux"
  MOOS_CXX_FLAGS="-fPIC"
fi

#===================================================================
# Part #1:  BUILD CORE
#===================================================================
cd "${MOOS_ABS_DIR}/MOOSCore"

echo "Invoking cmake..." `pwd`

cmake -DENABLE_EXPORT=ON -DUSE_ASYNC_COMMS=ON -DCMAKE_CXX_FLAGS="${MOOS_CXX_FLAGS}"  ./
#cmake -DENABLE_EXPORT=ON -DUSE_ASYNC_COMMS=OFF -DCMAKE_CXX_FLAGS="${MOOS_CXX_FLAGS}"  ./


echo ""; echo "Invoking make..." `pwd`; echo ""
make -j12 $@

echo "Moving MOOSCore executables into main MOOS bin directory"
mv bin/* ../../bin

cd ${INVOC_ABS_DIR}
echo "Returning to:" `pwd`

#===================================================================
# Part #2:  BUILD ESSENTIALS
#===================================================================
cd "${MOOS_ABS_DIR}/MOOSEssentials"

echo "Invoking cmake..." `pwd`
cmake -DCMAKE_CXX_FLAGS="${MOOS_CXX_FLAGS}"  ./

echo""; echo "Invoking make..." `pwd`; echo""
make -k -j8 $@

echo "Moving MOOSEssential executables into main MOOS bin directory"
mv bin/* ../../bin

cd ${INVOC_ABS_DIR}
echo "Returning to:" `pwd`

#===================================================================
# Part #3:  BUILD MOOS GUI TOOLS
#===================================================================
cd "${MOOS_ABS_DIR}/MOOSToolsUI"

echo "Invoking cmake..." `pwd`
cmake   -DBUILD_CONSOLE_TOOLS=ON                         \
        -DBUILD_GRAPHICAL_TOOLS=ON                       \
        -DBUILD_UPB=ON                       \
        -DCMAKE_CXX_FLAGS="${MOOS_CXX_FLAGS}" ./          
    
echo ""; echo "Invoking make..." `pwd`; echo ""
make -k -j $@

echo "Moving MOOSToolsUI executables into main MOOS bin directory"

mv bin/uMS.app/Contents/MacOS/uMS ../../bin
mv bin/uPlayBack.app/Contents/MacOS/uPlayBack ../../bin
mv bin/* ../../bin

#===================================================================
# Part #4:  BUILD MOOS GEODESY
#===================================================================
cd "${MOOS_ABS_DIR}/MOOSGeodesy"

echo "Invoking cmake..." `pwd`
cmake  -DCMAKE_CXX_FLAGS="${MOOS_CXX_FLAGS}"
    
echo ""; echo "Invoking make..." `pwd`; echo ""
make -j8 $@

cd ${INVOC_ABS_DIR}
