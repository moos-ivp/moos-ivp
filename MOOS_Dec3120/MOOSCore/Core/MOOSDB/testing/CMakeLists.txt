#this builds a test application which can be used
#to test and stress the MOOSDB

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/tests)

add_executable(async_test DBAsyncTest.cpp)
target_link_libraries(async_test MOOS)

add_executable(app_test DBAppTest.cpp)
target_link_libraries(app_test MOOS)

add_executable(io_test IOTest.cpp)
target_link_libraries(io_test MOOS)

add_executable(capture_test CaptureTest.cpp)
target_link_libraries(capture_test MOOS)

add_executable(cmp2sock cmp2sock.cpp)
target_link_libraries(cmp2sock MOOS)

add_executable(reg_test RegisterTest.cpp)
target_link_libraries(reg_test MOOS)
