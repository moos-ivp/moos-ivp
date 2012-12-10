#this builds a test application which can be used
#to test and stress the MOOSDB

set(EXECNAME1 umm)
set(EXECNAME2 async_test)
set(EXECNAME3 app_test)
set(EXECNAME4 queue_test)

#what files are needed?
SET(SRCS1 umm.cpp)
SET(SRCS2 DBAsyncTest.cpp)
SET(SRCS3 DBAppTest.cpp)
SET(SRCS4 QueueTest.cpp)

include_directories( ${MOOS_INCLUDE_DIRS} ${MOOS_DEPEND_INCLUDE_DIRS})
add_executable(${EXECNAME1} ${SRCS1} )
target_link_libraries(${EXECNAME1} ${MOOS_LIBRARIES} ${MOOS_DEPEND_LIBRARIES})

add_executable(${EXECNAME2} ${SRCS2} )
target_link_libraries(${EXECNAME2} ${MOOS_LIBRARIES} ${MOOS_DEPEND_LIBRARIES})

add_executable(${EXECNAME3} ${SRCS3} )
target_link_libraries(${EXECNAME3} ${MOOS_LIBRARIES} ${MOOS_DEPEND_LIBRARIES})

add_executable(${EXECNAME4} ${SRCS4} )
target_link_libraries(${EXECNAME4} ${MOOS_LIBRARIES} ${MOOS_DEPEND_LIBRARIES})
