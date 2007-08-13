FILE(GLOB SRC *.cpp)

OPTION(IVP_BUILD_pEchoVar "Build program pEchoVar" ON)

IF("${IVP_BUILD_pEchoVar}" STREQUAL "ON")
   ADD_EXECUTABLE(pEchoVar ${SRC})
   
   TARGET_LINK_LIBRARIES(pEchoVar 
   MOOS
   MOOSGen
   mbutil
   m
   pthread)
ENDIF("${IVP_BUILD_pEchoVar}" STREQUAL "ON")
