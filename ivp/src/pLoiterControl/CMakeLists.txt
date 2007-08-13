FILE(GLOB SRC *.cpp)

OPTION(IVP_BUILD_pLoiterControl "Build program pLoiterControl" ON)

IF("${IVP_BUILD_pLoiterControl}" STREQUAL "ON")
   ADD_EXECUTABLE(pLoiterControl ${SRC})
   
   TARGET_LINK_LIBRARIES(pLoiterControl 
   MOOS
   MOOSGen
   mbutil
   geometry
   m
   pthread)
ENDIF("${IVP_BUILD_pLoiterControl}" STREQUAL "ON")
