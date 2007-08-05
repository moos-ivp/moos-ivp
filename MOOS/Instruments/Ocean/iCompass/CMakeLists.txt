PROJECT(iCompass)

# next few lines are simply to insert icons into
# the console application on windows
IF(WIN32)
    SET(RESOURCES icon1.ico iCompass.rc)
ENDIF(WIN32)

#what files are needed?
SET(ICOMPASS_SRCS 
 CompassInstrument.cpp 
 CompassInstrument.h
 iCompassMain.cpp  
 resource.h
 ${RESOURCES})

# Add executable called iDepth
ADD_EXECUTABLE( iCompass ${ICOMPASS_SRCS})

# indicate how to link
TARGET_LINK_LIBRARIES(iCompass ${MOOSLIBS} ${PLATFORM_LIBS} )

#===============================================================================
# For installation and Debian packaging...
#===============================================================================

IF ("${MOOS_VERSION}" STREQUAL "")
   MESSAGE(STATUS "MOOS_VERSION not set, so not creating install component.")
ELSE("${MOOS_VERSION}" STREQUAL "")
   INSTALL(
      TARGETS iCompass
      DESTINATION bin/moos-${MOOS_VERSION}
      COMPONENT Moos_Instruments)
ENDIF("${MOOS_VERSION}" STREQUAL "")

#===============================================================================
