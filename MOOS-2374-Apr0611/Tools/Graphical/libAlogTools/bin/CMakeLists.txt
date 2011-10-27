project(alogToolsBin)

include_directories(${alogTools_INCLUDE_DIRS})

#add_executable(readIndexedAlog readIndexedAlog.cpp)
#target_link_libraries(readIndexedAlog ${alogTools_LIBRARIES})

add_executable(alogIndexWriter idxWriter.cpp)
target_link_libraries(alogIndexWriter ${alogTools_LIBRARIES})
