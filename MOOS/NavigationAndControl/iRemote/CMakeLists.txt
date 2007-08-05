PROJECT(iRemote)

# next few lines are simply to insert icons into
# the console application on windows
IF(WIN32)
    SET(RESOURCES icon1.ico iRemote.rc)
ENDIF(WIN32)

#what files are needed?
SET(IREMOTE_SRCS MOOSRemote.cpp MOOSRemote.h iRemoteMain.cpp  ${RESOURCES})

# Add executable called pLogger
ADD_EXECUTABLE( iRemote ${IREMOTE_SRCS})

# indicate how to link
TARGET_LINK_LIBRARIES(iRemote ${MOOSLIBS} ${PLATFORM_LIBS} )

#===============================================================================
# For installation and Debian packaging...
#===============================================================================

IF ("${MOOS_VERSION}" STREQUAL "")
   MESSAGE(STATUS "MOOS_VERSION not set, so not creating install component.")
ELSE("${MOOS_VERSION}" STREQUAL "")
   INSTALL(
      TARGETS iRemote
      DESTINATION bin/moos-${MOOS_VERSION}
      COMPONENT Moos_NavigationAndControl)
ENDIF("${MOOS_VERSION}" STREQUAL "")

#===============================================================================
