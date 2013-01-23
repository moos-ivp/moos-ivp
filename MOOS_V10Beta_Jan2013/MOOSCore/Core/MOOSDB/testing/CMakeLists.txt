#this builds a test application which can be used
#to test and stress the MOOSDB


include_directories( ${MOOS_INCLUDE_DIRS} ${MOOS_DEPEND_INCLUDE_DIRS})

add_executable(async_test DBAsyncTest.cpp )
target_link_libraries(async_test ${MOOS_LIBRARIES} ${MOOS_DEPEND_LIBRARIES})

add_executable(app_test DBAppTest.cpp )
target_link_libraries(app_test ${MOOS_LIBRARIES} ${MOOS_DEPEND_LIBRARIES})

add_executable(queue_test QueueTest.cpp )
target_link_libraries(queue_test ${MOOS_LIBRARIES} ${MOOS_DEPEND_LIBRARIES})


