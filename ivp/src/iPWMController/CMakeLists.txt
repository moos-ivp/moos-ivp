FILE(GLOB SRC *.cpp)

OPTION(IVP_BUILD_iPWMController "Build program iPWMController" ON)

IF("${IVP_BUILD_iPWMController}" STREQUAL "ON")
   ADD_EXECUTABLE(iPWMController ${SRC})
   
   TARGET_LINK_LIBRARIES(iPWMController 
   MOOS
   MOOSGen
   anrp_util
   m
   pthread)
ENDIF("${IVP_BUILD_iPWMController}" STREQUAL "ON")
