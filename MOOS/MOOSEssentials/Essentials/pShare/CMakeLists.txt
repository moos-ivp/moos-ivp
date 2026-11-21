#this builds the pScheduler application
set(EXECNAME pShare)

find_package(MOOS 10.0)

#what files are needed?
SET(SRCS   Share.cpp Listener.cpp Route.cpp ShareHelp.cpp pShareMain.cpp)

include_directories( ${${EXECNAME}_INCLUDE_DIRS} ${MOOS_INCLUDE_DIRS} ${MOOS_DEPEND_INCLUDE_DIRS})
add_executable(${EXECNAME} ${SRCS} )
target_link_libraries(${EXECNAME} ${MOOS_LIBRARIES} ${MOOS_DEPEND_LIBRARIES})

INSTALL(TARGETS ${EXECNAME}
  RUNTIME DESTINATION bin
)

#copy resources
# file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/pshare_test_scripts DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

if(IS_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/pshare_test_scripts)
  add_custom_command(TARGET pShare POST_BUILD
                     COMMAND ${CMAKE_COMMAND} -E copy_directory
                         ${CMAKE_CURRENT_SOURCE_DIR}/pshare_test_scripts ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/pshare_test_scripts)
else()
  message(WARNING " could not copy pshare test scripts to  ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
endif()