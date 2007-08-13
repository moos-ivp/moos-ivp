FILE(GLOB SRC *.cpp)

OPTION(IVP_BUILD_splug "Build program splug" ON)

IF("${IVP_BUILD_splug}" STREQUAL "ON")
   ADD_EXECUTABLE(splug ${SRC})
   
   TARGET_LINK_LIBRARIES(splug 
   mbutil
   m)
ENDIF("${IVP_BUILD_splug}" STREQUAL "ON")
