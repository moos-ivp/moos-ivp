PROJECT(iRelay)


#what files are needed?
SET(IRELAY_SRCS
 RelayBoard.cpp RelayBoard.h
 iRelayMain.cpp  
)

# Add executable called iDepth
ADD_EXECUTABLE( iRelay ${IRELAY_SRCS})

# indicate how to link
TARGET_LINK_LIBRARIES( iRelay ${MOOSLIBS} ${PLATFORM_LIBS} )

#===============================================================================
# For installation and Debian packaging...
#===============================================================================

IF ("${MOOS_VERSION}" STREQUAL "")
   MESSAGE(STATUS "MOOS_VERSION not set, so not creating install component.")
ELSE("${MOOS_VERSION}" STREQUAL "")
   INSTALL(
      TARGETS iRelay
      DESTINATION bin/moos-${MOOS_VERSION}
      COMPONENT Moos_Instruments)
ENDIF("${MOOS_VERSION}" STREQUAL "")

#===============================================================================
