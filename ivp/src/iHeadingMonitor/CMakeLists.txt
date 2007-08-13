FILE(GLOB SRC *.cpp)

OPTION(IVP_BUILD_iHeadingMonitor "Build program iHeadingMonitor" ON)

IF("${IVP_BUILD_iHeadingMonitor}" STREQUAL "ON")
   ADD_EXECUTABLE(iHeadingMonitor ${SRC})
   
   TARGET_LINK_LIBRARIES(iHeadingMonitor 
   MOOS
   MOOSGen
   mbutil
   m
   pthread)
ENDIF("${IVP_BUILD_iHeadingMonitor}" STREQUAL "ON")
