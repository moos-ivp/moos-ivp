PROJECT(iActuation)

# next few lines are simply to insert icons into
# the console application on windows
IF(WIN32)
    SET(RESOURCES icon1.ico iActuation.rc)
ENDIF(WIN32)

#what files are needed?
SET(IACTUATION_SRCS 
 MOOSActuation.cpp MOOSActuation.h
 MOOSActuationDriver.cpp MOOSActuationDriver.h
 MOOSASCDriver.cpp MOOSASCDriver.h
 MOOSBluefinDriver.cpp MOOSBluefinDriver.h
 MOOSJRKerrDriver.cpp MOOSJRKerrDriver.h
 MOOSSAILDriver.cpp MOOSSAILDriver.h
 iActuationMain.cpp 
 resource.h
 ${RESOURCES})

# Add executable called iDepth
ADD_EXECUTABLE( iActuation ${IACTUATION_SRCS})

# indicate how to link
TARGET_LINK_LIBRARIES(iActuation ${MOOSLIBS} ${PLATFORM_LIBS} )

#===============================================================================
# For installation and Debian packaging...
#===============================================================================

IF ("${MOOS_VERSION}" STREQUAL "")
   MESSAGE(STATUS "MOOS_VERSION not set, so not creating install component.")
ELSE("${MOOS_VERSION}" STREQUAL "")
   INSTALL(
      TARGETS iActuation
      DESTINATION bin/moos-${MOOS_VERSION}
      COMPONENT Moos_Instruments)
ENDIF("${MOOS_VERSION}" STREQUAL "")

#===============================================================================
