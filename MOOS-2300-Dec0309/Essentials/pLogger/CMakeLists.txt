PROJECT(pLogger)



# next few lines are simply to insert icons into
# the console application on windows
IF(WIN32)
    SET(RESOURCES icon1.ico pLogger.rc)
ENDIF(WIN32)

#what files are needed?
SET(PLOGGER_SRCS
 MOOSLogger.cpp
 MOOSLogger.h
 pLoggerMain.cpp
 Zipper.cpp
 Zipper.h
 ${RESOURCES})


FIND_PACKAGE(ZLIB QUIET)
IF (ZLIB_FOUND)
    ADD_DEFINITIONS(-DZLIB_FOUND)
    INCLUDE_DIRECTORIES(${ZLIB_INCLUDE_DIR})
    MESSAGE(STATUS "Cmake FindZLIB: using ZLIB includes at: ${ZLIB_INCLUDE_DIR}")
    MESSAGE(STATUS "Cmake FindZLIB: using ZLIB libraries: ${ZLIB_LIBRARIES}")
ELSE(ZLIB_FOUND)
	SET(ZLIB_LIBRARIES "")
ENDIF (ZLIB_FOUND)


# Add executable called pLogger
ADD_EXECUTABLE( pLogger ${PLOGGER_SRCS})

# indicate how to link
TARGET_LINK_LIBRARIES(pLogger ${MOOSLIBS} ${PLATFORM_LIBS} ${ZLIB_LIBRARIES} )

INSTALL(TARGETS
pLogger
RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin
)