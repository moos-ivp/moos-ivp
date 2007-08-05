PROJECT(MOOSDB)

# next few lines are simply to insert icons into
# the console application on windows
IF(WIN32)
    SET(RESOURCES icon1.ico MOOSDB.rc)
ENDIF(WIN32)

#what files are needed?
SET(MOOSDB_SRCS MOOSDB.cpp MOOSDBMain.cpp MOOSDBVar.cpp MOOSRegisterInfo.cpp
    MOOSDB.h MOOSDBVar.h MOOSRegisterInfo.h
    ${RESOURCES})

# Add executable called MOOSDB
ADD_EXECUTABLE( MOOSDB ${MOOSDB_SRCS})

# indicate how to link
# MESSAGE(FATAL_ERROR "MOOSLIBS = ${MOOSLIBS}")
SET(MOOSLIBS MOOS MOOSGen)

TARGET_LINK_LIBRARIES(MOOSDB ${MOOSLIBS} ${PLATFORM_LIBS} )

#===============================================================================
# For installation and Debian packaging...
#===============================================================================

INSTALL(
   TARGETS MOOSDB
   DESTINATION bin/moos-${MOOS_VERSION}
   COMPONENT Moos_Core)

#===============================================================================
