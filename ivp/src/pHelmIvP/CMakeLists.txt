FILE(GLOB SRC *.cpp)

OPTION(IVP_BUILD_pHelmIvP "Build program pHelmIvP" ON)

IF("${IVP_BUILD_pHelmIvP}" STREQUAL "ON")
   ADD_EXECUTABLE(pHelmIvP ${SRC})
   
   TARGET_LINK_LIBRARIES(pHelmIvP 
   MOOS
   MOOSGen
   helmivp
   behaviors-marine
   behaviors-sandbox
   bhvutil	
   behaviors
   mbutil 
   ivpbuild
   ivpcore 
   geometry
   logic
   m
   pthread)
ENDIF("${IVP_BUILD_pHelmIvP}" STREQUAL "ON")
