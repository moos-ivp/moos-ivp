PROJECT(iDVL)

# next few lines are simply to insert icons into
# the console application on windows
IF(WIN32)
    SET(RESOURCES icon1.ico iDVL.rc)
ENDIF(WIN32)

#what files are needed?
SET(IDVL_SRCS 
 DVLInstrument.cpp DVLInstrument.h
 iDVLMain.cpp  
 resource.h
 ${RESOURCES})

# Add executable called iDepth
ADD_EXECUTABLE( iDVL ${IDVL_SRCS})

# indicate how to link
TARGET_LINK_LIBRARIES(iDVL ${MOOSLIBS} ${PLATFORM_LIBS} )

#===============================================================================
# For installation and Debian packaging...
#===============================================================================

IF ("${MOOS_VERSION}" STREQUAL "")
   MESSAGE(STATUS "MOOS_VERSION not set, so not creating install component.")
ELSE("${MOOS_VERSION}" STREQUAL "")
   INSTALL(
      TARGETS iDVL
      DESTINATION bin/moos-${MOOS_VERSION}
      COMPONENT Moos_Instruments)
ENDIF("${MOOS_VERSION}" STREQUAL "")

#===============================================================================
