# - Find Matlab includes and libraries
#
# This module defines
#  MATLAB_DIR         - Matlab directory
#
# Advanced options (usually deduced from the above, but can be set manually if desired)
#  MATLAB_INCLUDE_DIR - include path for mex.h, engine.h, etc.
#  MATLAB_LIBRARY_DIR - path to libmex, libmx, libeng etc.
#  MATLAB_LIBRARIES   - libmex, libmx, libeng, libmat etc.
#  MATLAB_MEX_SUFFIX  - platform dependent mex shared library file suffix.
#  MATLAB_MEX_LDFLAGS - extra flags needed to compile mex shared libraries.  
#  MATLAB_FOUND       - indicate that we did indeed find matlab. 
#
# This module is a replacement for the FindMatlab shipped with CMake...  Note
# in particular that we aim to support compiling our own mex modules, which means
# this script needs to duplicate the work of the Matlab mex script.  Can anyone
#
# PNM, can you tell me again why we're not using mex directly?  GTS
#

SET(MATLAB_DIR_DESCRIPTION "directory containing MATLAB - e.g /opt/matlab or c:\\MATLAB")

SET( CMAKE_ALLOW_LOOSE_LOOP_CONSTRUCTS 1 )

# Be quite if already in cache.
IF( MATLAB_FOUND )
  SET( MATLAB_FIND_QUIETLY 0 )
ENDIF()

# Find representative header to define matlab root dir.
FIND_PATH( MATLAB_DIR extern/include/mex.h
		PATHS
		/opt/matlab
		/usr/local/matlab-7sp1
		/opt/matlab-7sp1
		C:/Program\ Files/MATLAB/R2007b
		$ENV{HOME}/matlab7_64
		$ENV{HOME}/matlab-7sp1
		$ENV{HOME}/redhat-matlab
		$ENV{HOME}/matlab
		$ENV{HOME}/usr/share/matlab
		C:/matlabR12
		ENV MATLAB_PATH
		DOC "The ${MATLAB_DIR_DESCRIPTION}"
)

# Found matlab dir, from which we can deduce everything else.
IF( MATLAB_DIR )
	SET( MATLAB_INCLUDE_DIR ${MATLAB_DIR}/extern/include CACHE PATH "Matlab includes" FORCE)
ENDIF()

IF(UNIX)
    SET( MATLAB_LIBRARIES mex eng mat mx CACHE STRING "Matlab libraries" FORCE )
    IF("${CMAKE_SYSTEM_NAME}" MATCHES "Darwin")
        IF( ${CMAKE_OSX_ARCHITECTURES} MATCHES "i386" )
            SET( MATLAB_ARCH maci )
            SET( MATLAB_MEX_SUFFIX .mexmaci CACHE STRING "Mex shared library file suffix" )
            SET( MATLAB_CXX_FLAGS
                    "${CMAKE_CXX_FLAGS} -flat_namespace -undefined suppress" CACHE INTERNAL 
                    "extra CFLAGS to suppress linker errors in Mac OS X" )
        ELSE()
            SET( MATLAB_ARCH mac )
            SET( MATLAB_MEX_SUFFIX .mexmac CACHE STRING "Mex shared library file suffix" )
        ENDIF()
    ENDIF()
    IF("${CMAKE_SYSTEM_NAME}" MATCHES "Linux")
        SET( MATLAB_ARCH glnx86 )
        SET( MATLAB_MEX_SUFFIX .mexglx )
    ENDIF()
ENDIF()

IF(WIN32)
    SET( MATLAB_LIBRARIES libmex libeng libmat libmx CACHE STRING "Matlab libraries" FORCE)
    SET( MATLAB_ARCH win32 )
    
    # For newer versions of Matlab, this should be .mex32 or somesuch (http://tinyurl.com/32r88r)
    SET( MATLAB_MEX_SUFFIX .dll )
ENDIF()

SET( LIB_SEARCH_PATHS
    ${MATLAB_DIR}/bin/${MATLAB_ARCH} # for .so's on unix (can't link against .dll's in windows, however)
    ${MATLAB_DIR}/extern/lib/${MATLAB_ARCH}
    ${MATLAB_DIR}/extern/lib/${MATLAB_ARCH}/microsoft/msvc60
	${MATLAB_DIR}/extern/lib/${MATLAB_ARCH}/microsoft
	)

FIND_LIBRARY( MATLAB_LIBRARY_DIR NAMES ${MATLAB_LIBRARIES}
	PATHS ${LIB_SEARCH_PATHS}
	ENV CPATH 
	DOC "The directory where the Matlab libraries are installed."
	FORCE
)

MESSAGE( "lib ${MATLAB_LIBRARIES}: ${MATLAB_LIBRARY_DIR} in ${SEARCH_PATHS}")
GET_FILENAME_COMPONENT( MATLAB_LIBRARY_DIR ${MATLAB_LIBRARY_DIR} PATH CACHE )

IF( MATLAB_DIR AND MATLAB_INCLUDE_DIR AND MATLAB_LIBRARY_DIR )
    SET( MATLAB_FOUND TRUE CACHE INTERNAL "")
ELSE()
    SET( MATLAB_FOUND FALSE CACHE INTERNAL "")
ENDIF()

IF( MATLAB_FOUND )
  IF( NOT MATLAB_FIND_QUIETLY )
    MESSAGE( STATUS "MATLAB_INCLUDE_DIR: ${MATLAB_INCLUDE_DIR}" )
    MESSAGE( STATUS "MATLAB_LIBRARIES: ${MATLAB_LIBRARIES}" )
    MESSAGE( STATUS "MATLAB_LIBRARY_DIR: ${MATLAB_LIBRARY_DIR}" )
    MESSAGE( STATUS "MATLAB_MEX_LDFLAGS: ${MATLAB_MEX_LDFLAGS}" )
    MESSAGE( STATUS "MATLAB_MEX_SUFFIX: ${MATLAB_MEX_SUFFIX}" )
    MESSAGE( STATUS "Found MATLAB libraries: ${MATLAB_LIBRARY_DIR}" )

  ENDIF( NOT MATLAB_FIND_QUIETLY )
ELSE()
  IF( Matlab_MRG_FIND_REQUIRED )
    IF( NOT MATLAB_DIR )
        MESSAGE( FATAL_ERROR "Could NOT find MATLAB -- please provide the directory containing MATLAB - e.g /opt/matlab or c:\\MATLAB")
    ELSE()
        MESSAGE( STATUS "Found MATLAB dir: ${MATLAB_DIR}")
    ENDIF()

    IF( NOT MATLAB_INCLUDE_DIR )
        MESSAGE( FATAL_ERROR "Could NOT find MATLAB headers -- please provide the directory containing MATLAB headers- e.g /opt/matlab/extern/include")
    ELSE()
        MESSAGE( STATUS "Found MATLAB include dir: ${MATLAB_INCLUDE_DIR}")
    ENDIF()

    IF( NOT MATLAB_LIBRARY_DIR )
        MESSAGE( FATAL_ERROR "Could NOT find MATLAB libraries -- please provide the directory containing MATLAB libraries - e.g /opt/matlab/bin/glnx86" )
    ELSE()
        MESSAGE( STATUS "Found MATLAB library dir: ${MATLAB_LIBRARY_DIR}")
    ENDIF()
  ENDIF()
ENDIF()

MARK_AS_ADVANCED(
	MATLAB_INCLUDE_DIR
	MATLAB_LIBRARY_DIR
	MATLAB_LIBRARIES
	MATLAB_MEX_SUFFIX
	MATLAB_MEX_LDFLAGS
	)

