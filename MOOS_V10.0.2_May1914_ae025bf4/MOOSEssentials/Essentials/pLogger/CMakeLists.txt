
#this builds the pLogger application
set(EXECNAME pLogger)

find_package(MOOS 10)

#what files are needed?
SET(SRCS  MOOSLogger.cpp pLoggerMain.cpp Zipper.cpp)

FIND_PACKAGE(ZLIB QUIET)
IF (ZLIB_FOUND)
    ADD_DEFINITIONS(-DZLIB_FOUND)
    INCLUDE_DIRECTORIES(${ZLIB_INCLUDE_DIR})
    MESSAGE(STATUS "Cmake FindZLIB: using ZLIB includes at: ${ZLIB_INCLUDE_DIR}")
    MESSAGE(STATUS "Cmake FindZLIB: using ZLIB libraries: ${ZLIB_LIBRARIES}")
ELSE(ZLIB_FOUND)
    SET(ZLIB_LIBRARIES "")
ENDIF (ZLIB_FOUND)

include_directories( ${${EXECNAME}_INCLUDE_DIRS} ${MOOS_INCLUDE_DIRS} ${MOOS_DEPEND_INCLUDE_DIRS} ${ZLIB_INCLUDE_DIRS})
add_executable(${EXECNAME} ${SRCS} )
target_link_libraries(${EXECNAME} ${MOOS_LIBRARIES} ${MOOS_DEPEND_LIBRARIES} ${ZLIB_LIBRARIES})

INSTALL(TARGETS ${EXECNAME}
  RUNTIME DESTINATION bin
)


