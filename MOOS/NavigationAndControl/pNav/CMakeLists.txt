PROJECT(pNav)

# next few lines are simply to insert icons into
# the console application on windows
IF(WIN32)
    SET(RESOURCES icon1.ico pNav.rc)
ENDIF(WIN32)

#what files are needed?
SET(PNAV_SRCS MOOSNavigator.cpp pNavMain.cpp MOOSPriorityInput.cpp MOOSNavigator.h MOOSPriorityInput.h ${RESOURCES})

SET( BUILD_NEWMAT ON CACHE BOOL "Build Newmat" FORCE)
INCLUDE_DIRECTORIES(${THIRDPARTY_DIR} ${THIRDPARTY_DIR}/newmat)

# Add executable called pLogger
ADD_EXECUTABLE( pNav ${PNAV_SRCS})

# indicate how to link
TARGET_LINK_LIBRARIES(pNav ${MOOSLIBS} ${PLATFORM_LIBS}  MOOSNav newmat )

#===============================================================================
# For installation and Debian packaging...
#===============================================================================

IF ("${MOOS_VERSION}" STREQUAL "")
   MESSAGE(STATUS "MOOS_VERSION not set, so not creating install component.")
ELSE("${MOOS_VERSION}" STREQUAL "")
   INSTALL(
      TARGETS pNav
      DESTINATION bin/moos-${MOOS_VERSION}
      COMPONENT Moos_NavigationAndControl)
ENDIF("${MOOS_VERSION}" STREQUAL "")

#===============================================================================
