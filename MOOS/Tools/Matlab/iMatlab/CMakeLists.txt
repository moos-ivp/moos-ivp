PROJECT(iMatlab)

INCLUDE_DIRECTORIES(${VXL_INCLUDE_DIRECTORIES})
ADD_DEFINITIONS(-DMATLAB_MEX_FILE)




IF(WIN32)
	SET(DEFAULT_MATLAB_PATH C:/matlabR12)
	SET(MATLABLIBS libmx.lib libmex.lib)
	SET( MATLAB_PATH ${DEFAULT_MATLAB_PATH} CACHE PATH "Enter Matlab Path")
	LINK_DIRECTORIES( ${MATLAB_PATH}/extern/lib/win32/microsoft/msvc60 ${MATLAB_PATH}/extern/lib/win32/microsoft/ ) 
ELSE(WIN32)
	SET(MATLAB_PATH $ENV{MATLAB_PATH})
	SET(CMAKE_CXX_FLAGS  " ${CMAKE_CXX_FLAGS} -fPIC")
	SET( MATLAB_PATH ${DEFAULT_MATLAB_PATH} CACHE PATH "Matlab Path from MATLAB_PATH environment variable e.g /usr/local/matlab")
	SET(INNAME "libiMatlab"${CMAKE_SHLIB_SUFFIX})
	SET(OUTNAME "iMatlab.mexglx")
	LINK_DIRECTORIES( ${MATLAB_PATH}/extern/lib/glnx86 )
ENDIF(WIN32)


INCLUDE_DIRECTORIES(${MATLAB_PATH}/extern/include )
LINK_LIBRARIES( ${MATLABLIBS} ${MOOSLIBS} ${PLATFORM_LIBS} ${VNLLIBS})
LINK_DIRECTORIES(${LIBRARY_OUTPUT_PATH} ${VXLLIBDIR})

IF(WIN32)
SET(IMATLAB_SRCS
    iMatlab.cpp
    mexVNLHelpers.cpp
    mexVNLHelpers.h
    iMatlab.def
    iMatlab.help
)
ELSE(WIN32)
SET(IMATLAB_SRCS
    iMatlab.cpp
    mexVNLHelpers.cpp
    mexVNLHelpers.h)
ENDIF(WIN32)


ADD_LIBRARY(iMatlab SHARED ${IMATLAB_SRCS})



# Copy:
  ADD_CUSTOM_COMMAND(
  TARGET iMatlab
  POST_BUILD
  COMMAND ${CMAKE_COMMAND}
  ARGS -E copy
  ${iMatlab_SOURCE_DIR}/iMatlab.help ${LIBRARY_OUTPUT_PATH}/${CMAKE_CFG_INTDIR}/iMatlab.help	)

IF(WIN32)
ELSE(WIN32)
	ADD_CUSTOM_COMMAND(
				   TARGET iMatlab                   
				   POST_BUILD
                   COMMAND ${CMAKE_COMMAND}
#				   ARGS -E echo	${LIBRARY_OUTPUT_PATH}/${INNAME}
                   ARGS -E  copy	${LIBRARY_OUTPUT_PATH}/libiMatlab.so  ${LIBRARY_OUTPUT_PATH}/iMatlab.mexglx
#                   ARGS -E  copy	txt	lala
                   )
ENDIF(WIN32)

#===============================================================================
# For installation and Debian packaging...
#===============================================================================

# Note: building this component imposes a unique requirement on the user doing
# the building: he must have Matlab.
#
# Persons merely building MOOS can avoid this issue by setting the 
# BUILD_MATLAB_TOOLS CMake variable to OFF.  But we really don't want the 
# situation where users disable the building of iMatlab, and then go on to
# construct the moos-tools Debian package.  If that happened, we'd have floating
# around some copies of the moos-tools Debian package that (errantly) lacked
# a library (iMatlab) that the package was supposed to contain.
#
# So to avoid all that potential confusion, I'm putting iMatlab into its own 
# Debian package.  That in turn forces me to put iMatlab into its own CMake
# installation component ("Moos_Tools_Matlab"). -CJC

IF(${BUILD_MATLAB_TOOLS})
   INSTALL(
      TARGETS iMatlab
      DESTINATION bin/moos-${MOOS_VERSION}
      COMPONENT Moos_Tools_Matlab)
ELSE(${BUILD_MATLAB_TOOLS})
   MESSAGE(STATUS 
      "Warning: Not building Matlab tools, because the "
      "BUILD_MATLAB_TOOLS variable isn't set to 'ON'.  So the 'Moos_Tools' "
      "install component (and associated Debian package file) won't contain "
      "the iMatlab shared library.")
ENDIF(${BUILD_MATLAB_TOOLS})

#===============================================================================
