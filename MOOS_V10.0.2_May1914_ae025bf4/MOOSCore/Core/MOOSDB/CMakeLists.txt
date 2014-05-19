set(EXECNAME MOOSDB)

# next few lines are simply to insert icons into
# the console application on windows
IF(WIN32)
    SET(RESOURCES icon1.ico MOOSDB.rc)
ENDIF(WIN32)

#what files are needed?
SET(SRCS  MOOSDBMain.cpp
    ${RESOURCES})

include(PlatformDefines)

# An option to disable names lookup in MOOS server.
SET(DISABLE_NAMES_LOOKUP OFF CACHE BOOL "Disable client names lookup in MOOSDB")
IF(DISABLE_NAMES_LOOKUP)
    ADD_DEFINITIONS(-DMOOS_DISABLE_NAMES_LOOKUP)
ENDIF(DISABLE_NAMES_LOOKUP)

foreach(DEFINE ${PROJECT_EXPORT_DEFINES})        
    ADD_DEFINITIONS(-D${DEFINE})        
    message(STATUS "adding -D${DEFINE} as required by MOOS")
endforeach()


include_directories( ${${EXECNAME}_INCLUDE_DIRS} ${MOOS_INCLUDE_DIRS} ${MOOS_DEPEND_INCLUDE_DIRS})
add_executable(${EXECNAME} ${SRCS} )
target_link_libraries(${EXECNAME} ${MOOS_LIBRARIES} ${MOOS_DEPEND_LIBRARIES})



INSTALL(TARGETS ${EXECNAME}
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
)

add_subdirectory(testing)

