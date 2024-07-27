set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/tests/lib)

add_executable(periodic_test PeriodicEventTest.cpp)
target_link_libraries(periodic_test MOOS)

add_executable(queue_test QueueTest.cpp)
target_link_libraries(queue_test MOOS)

add_executable(suicide_test SuicideTest.cpp)
target_link_libraries(suicide_test MOOS)

add_executable(binding_test BindingTest.cpp)
target_link_libraries(binding_test MOOS)


