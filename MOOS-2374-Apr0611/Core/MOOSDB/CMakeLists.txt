PROJECT(MOOSDB)

# next few lines are simply to insert icons into
# the console application on windows
IF(WIN32)
    SET(RESOURCES icon1.ico MOOSDB.rc)
ENDIF(WIN32)

#what files are needed?
SET(MOOSDB_SRCS MOOSDB.cpp MOOSDBMain.cpp MOOSDBVar.cpp MOOSRegisterInfo.cpp
    MOOSDB.h MOOSDBVar.h MOOSRegisterInfo.h MOOSDBHTTPServer.cpp MOOSDBHTTPServer.h HTTPConnection.cpp HTTPConnection.h
    ${RESOURCES})

#do we want a webserver compiled in?
SET(DB_HAS_HTTP FALSE CACHE BOOL "DB serves HTTP (experimental)")
IF (DB_HAS_HTTP)
    ADD_DEFINITIONS(-DMOOSDB_HAS_WEBSERVER)
ENDIF (DB_HAS_HTTP)


# Add executable called MOOSDB
ADD_EXECUTABLE( MOOSDB ${MOOSDB_SRCS})

# indicate how to link
TARGET_LINK_LIBRARIES(MOOSDB ${MOOSLIBS} ${PLATFORM_LIBS} )

INSTALL(TARGETS
MOOSDB
RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin
)

