PROJECT(pAntler)

IF(WIN32)
    SET(RESOURCES icon1.ico pAntler.rc)
ENDIF(WIN32)


#build a list of files to compile
SET(ANTLER_SRCS AntlerMain.cpp )
IF(WIN32)
    SET(ANTLER_SRCS ${ANTLER_SRCS} XPCProcess.cpp XPCProcessAttrib.cpp XPCProcess.h XPCProcessAttrib.h ${RESOURCES})
ENDIF(WIN32)

# Add executable
ADD_EXECUTABLE( pAntler ${ANTLER_SRCS})

# Link the executable
TARGET_LINK_LIBRARIES(pAntler ${MOOSLIBS} ${PLATFORM_LIBS} )

#===============================================================================
# For installation and Debian packaging...
#===============================================================================

IF ("${MOOS_VERSION}" STREQUAL "")
   MESSAGE(STATUS "MOOS_VERSION not set, so not creating install component.")
ELSE("${MOOS_VERSION}" STREQUAL "")
   INSTALL(
      TARGETS pAntler
      DESTINATION bin/moos-${MOOS_VERSION}
      COMPONENT Moos_Essentials)
ENDIF("${MOOS_VERSION}" STREQUAL "")

#===============================================================================
