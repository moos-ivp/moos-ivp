
foreach(DEFINE ${PROJECT_EXPORT_DEFINES})        
    ADD_DEFINITIONS(-D${DEFINE})        
    message(STATUS "adding -D${DEFINE} as required by MOOS")
endforeach()


add_subdirectory(umm)
add_subdirectory(ktm)
add_subdirectory(mtm)
add_subdirectory(mqos)
