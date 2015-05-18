#!/bin/bash

BUILD_TYPE="Release"
CMD_ARGS=""

for ARGI; do
    if [ "${ARGI}" = "--help" -o "${ARGI}" = "-h" ] ; then
        printf "%s [SWITCHES]  \n" $0
        printf "  --help, -h                                   \n"
        printf "  --debug, -d                                  \n"
        printf "  --release, -r                                \n"
        printf "Any other switches passed directly to \"make\" \n"
        printf "Recommended:                                   \n"
        printf " -j12   Speed up compile on multi-core machines. \n"
        printf " -k     Keep building after failed component.    \n"
        printf " clean  Clean/remove any previous build.         \n"
        exit 0;
    elif [ "${ARGI}" = "--debug" -o "${ARGI}" = "-d" ] ; then
        BUILD_TYPE="Debug"
    elif [ "${ARGI}" = "--release" -o "${ARGI}" = "-r" ] ; then
        BUILD_TYPE="Release"
    else
	CMD_ARGS=$CMD_ARGS" "$ARGI
    fi
done

INVOC_ABS_DIR="`pwd`"
echo "  INVOC_ABS_DIR: " ${INVOC_ABS_DIR}

# Setup C and C++ Compiler flags for Mac and Linux. 
if [ "`uname`" == "Darwin" ] ; then
  echo "Building MOOS for Apple"
else
  echo "Building MOOS for Linux"
  MOOS_CXX_FLAGS="-fPIC -Wno-long-long"
fi

#===================================================================
# Part #1:  BUILD CORE
#      -DUPDATE_GIT_VERSION_INFO=OFF                          \
#===================================================================
cd "${INVOC_ABS_DIR}/MOOS/MOOSCore"

echo "Invoking cmake..." `pwd`
cmake -DENABLE_EXPORT=ON                                     \
      -DUSE_ASYNC_COMMS=ON                                   \
      -DTIME_WARP_AGGLOMERATION_CONSTANT=0.4                 \
      -DCMAKE_BUILD_TYPE=${BUILD_TYPE}                       \
      -DCMAKE_RUNTIME_OUTPUT_DIRECTORY=${INVOC_ABS_DIR}/bin  \
      -DCMAKE_CXX_FLAGS="${MOOS_CXX_FLAGS}"  ./

echo ""; echo "Invoking make..." `pwd`; echo ""
make  -j12 ${CMD_ARGS}

#===================================================================
# Part #2:  BUILD ESSENTIALS
#===================================================================
cd "${INVOC_ABS_DIR}/MOOS/MOOSEssentials"

echo "Invoking cmake..." `pwd`
cmake -DCMAKE_BUILD_TYPE=${BUILD_TYPE}                       \
      -DCMAKE_RUNTIME_OUTPUT_DIRECTORY=${INVOC_ABS_DIR}/bin  \
      -DCMAKE_CXX_FLAGS="${MOOS_CXX_FLAGS}" ./

echo""; echo "Invoking make..." `pwd`; echo""
make -j12 ${CMD_ARGS}


#===================================================================
# Part #3:  BUILD MOOS GUI TOOLS
#===================================================================
cd "${INVOC_ABS_DIR}/MOOS/MOOSToolsUI"

echo "Invoking cmake..." `pwd`
cmake -DBUILD_CONSOLE_TOOLS=ON                               \
      -DBUILD_GRAPHICAL_TOOLS=ON                             \
      -DBUILD_UPB=ON                                         \
      -DCMAKE_BUILD_TYPE=${BUILD_TYPE}                       \
      -DCMAKE_RUNTIME_OUTPUT_DIRECTORY=${INVOC_ABS_DIR}/bin  \
      -DCMAKE_CXX_FLAGS="${MOOS_CXX_FLAGS}" ./          
    
echo ""; echo "Invoking make..." `pwd`; echo ""
make -j12 ${CMD_ARGS}

#===================================================================
# Part #4:  BUILD PROJ4
#===================================================================
cd "${INVOC_ABS_DIR}/MOOS/proj-4.8.0"


if [ ! -e lib/libproj.dylib ]; then
    if [ ! -e lib/libproj.a ]; then
	echo "Building Proj4. MOOSGeodesy now uses Proj4 with MOOSGeodesy wrapper"
	./configure 
	make 
	make install 
	echo "Done Building Proj4."
    fi
fi


#===================================================================
# Part #5:  BUILD MOOS GEODESY
#===================================================================
cd "${INVOC_ABS_DIR}/MOOS/MOOSGeodesy"

PROJ4_INCLUDE_DIR="${INVOC_ABS_DIR}/MOOS/proj-4.8.0/include"
PROJ4_LIB_DIR="${INVOC_ABS_DIR}/MOOS/proj-4.8.0/lib"

echo "PROJ4 LIB DIR: " $PROJ4_LIB_DIR


echo "Invoking cmake..." `pwd`
cmake -DCMAKE_CXX_FLAGS="${MOOS_CXX_FLAGS}"         \
      -DPROJ4_INCLUDE_DIRS=${PROJ4_INCLUDE_DIR}     \
      -DPROJ4_LIB_PATH=${PROJ4_LIB_DIR} 

    
echo ""; echo "Invoking make..." `pwd`; echo ""
make -j12 $@

cd ${INVOC_ABS_DIR}
