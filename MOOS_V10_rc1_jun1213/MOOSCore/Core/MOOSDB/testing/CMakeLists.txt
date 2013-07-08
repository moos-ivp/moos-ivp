#this builds a test application which can be used
#to test and stress the MOOSDB

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/tests)

include_directories( ${MOOS_INCLUDE_DIRS} ${MOOS_DEPEND_INCLUDE_DIRS})

add_executable(async_test DBAsyncTest.cpp )
target_link_libraries(async_test ${MOOS_LIBRARIES} ${MOOS_DEPEND_LIBRARIES})

add_executable(app_test DBAppTest.cpp )
target_link_libraries(app_test ${MOOS_LIBRARIES} ${MOOS_DEPEND_LIBRARIES})

add_executable(io_test IOTest.cpp )
target_link_libraries(io_test ${MOOS_LIBRARIES} ${MOOS_DEPEND_LIBRARIES})

add_executable(capture_test CaptureTest.cpp )
target_link_libraries(capture_test ${MOOS_LIBRARIES} ${MOOS_DEPEND_LIBRARIES})

add_executable(cmp2sock cmp2sock.cpp )
target_link_libraries(cmp2sock ${MOOS_LIBRARIES} ${MOOS_DEPEND_LIBRARIES})

add_executable(reg_test RegisterTest.cpp )
target_link_libraries(reg_test ${MOOS_LIBRARIES} ${MOOS_DEPEND_LIBRARIES})
