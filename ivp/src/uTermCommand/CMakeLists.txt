FILE(GLOB SRC *.cpp)

OPTION(IVP_BUILD_uTermCommand "Build program uTermCommand" ON)

IF("${IVP_BUILD_uTermCommand}" STREQUAL "ON")
   ADD_EXECUTABLE(uTermCommand ${SRC})
   
   TARGET_LINK_LIBRARIES(uTermCommand 
   MOOS
   MOOSGen
   MOOSUtility
   mbutil
   genutil
   m
   pthread)
ENDIF("${IVP_BUILD_uTermCommand}" STREQUAL "ON")
