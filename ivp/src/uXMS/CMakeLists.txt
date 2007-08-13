FILE(GLOB SRC *.cpp)

OPTION(IVP_BUILD_uXMS "Build program uXMS" ON)

IF("${IVP_BUILD_uXMS}" STREQUAL "ON")
   ADD_EXECUTABLE(uXMS ${SRC})
   
   TARGET_LINK_LIBRARIES(uXMS 
   MOOS
   MOOSGen
   genutil
   mbutil
   m
   pthread)
ENDIF("${IVP_BUILD_uXMS}" STREQUAL "ON")
