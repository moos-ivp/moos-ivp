PROJECT(iDepth)

# next few lines are simply to insert icons into
# the console application on windows
IF(WIN32)
    SET(RESOURCES icon1.ico iDepth.rc)
ENDIF(WIN32)

#what files are needed?
SET(IDEPTH_SRCS 
 DepthInstrument.cpp DepthInstrument.h
 iDepthMain.cpp  
 MOOSDepthSensor.cpp  MOOSDepthSensor.h
 MOOSConsiDepthSensor.cpp  MOOSConsiDepthSensor.h
 MOOSParaSciDepthSensor.cpp  MOOSParaSciDepthSensor.h
 resource.h
 ${RESOURCES})

# Add executable called iDepth
ADD_EXECUTABLE( iDepth ${IDEPTH_SRCS})

# indicate how to link
TARGET_LINK_LIBRARIES(iDepth ${MOOSLIBS} ${PLATFORM_LIBS} )

#===============================================================================
# For installation and Debian packaging...
#===============================================================================

IF ("${MOOS_VERSION}" STREQUAL "")
   MESSAGE(STATUS "MOOS_VERSION not set, so not creating install component.")
ELSE("${MOOS_VERSION}" STREQUAL "")
   INSTALL(
      TARGETS iDepth
      DESTINATION bin/moos-${MOOS_VERSION}
      COMPONENT Moos_Instruments)
ENDIF("${MOOS_VERSION}" STREQUAL "")

#===============================================================================
