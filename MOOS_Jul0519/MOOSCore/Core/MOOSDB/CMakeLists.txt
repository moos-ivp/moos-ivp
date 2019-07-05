set(EXECNAME MOOSDB)

# next few lines are simply to insert icons into
# the console application on windows
if(WIN32)
    set(RESOURCES icon1.ico MOOSDB.rc)
endif()

#what files are needed?
set(SRCS  MOOSDBMain.cpp
    ${RESOURCES})

# An option to disable names lookup in MOOS server.
set(DISABLE_NAMES_LOOKUP OFF CACHE BOOL "Disable client names lookup in MOOSDB")
if(DISABLE_NAMES_LOOKUP)
    add_definitions(-DMOOS_DISABLE_NAMES_LOOKUP)
    add_definitions(-DMOOS_DISABLE_XPCTCP_NAME_LOOKUP)
endif()

include_directories(${${EXECNAME}_INCLUDE_DIRS})
add_executable(${EXECNAME} ${SRCS})
target_link_libraries(${EXECNAME} MOOS)

install(TARGETS ${EXECNAME}
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
)

add_subdirectory(testing)

