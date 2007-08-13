FILE(GLOB SRC *.cpp)

OPTION(IVP_BUILD_pFunctionSender "Build program pFunctionSender" ON)

IF("${IVP_BUILD_pFunctionSender}" STREQUAL "ON")
   ADD_EXECUTABLE(pFunctionSender ${SRC})
   
   TARGET_LINK_LIBRARIES(pFunctionSender 
   MOOS
   MOOSGen
   ivpbuild
   mbutil
   ivpcore
   m
   pthread)
ENDIF("${IVP_BUILD_pFunctionSender}" STREQUAL "ON")
