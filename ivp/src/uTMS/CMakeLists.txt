FILE(GLOB SRC *.cpp)

OPTION(IVP_BUILD_uTMS "Build program uTMS" ON)

IF("${IVP_BUILD_uTMS}" STREQUAL "ON")
   ADD_EXECUTABLE(uTMS ${SRC})
   
   TARGET_LINK_LIBRARIES(uTMS 
   MOOS
   MOOSGen
   genutil
   mbutil
   m
   pthread)
ENDIF("${IVP_BUILD_uTMS}" STREQUAL "ON")
