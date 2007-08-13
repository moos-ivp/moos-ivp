FILE(GLOB SRC *.cpp)

OPTION(IVP_BUILD_p2BTracker "Build program p2BTracker" ON)

IF("${IVP_BUILD_p2BTracker}" STREQUAL "ON")
   ADD_EXECUTABLE(p2BTracker ${SRC})
   
   TARGET_LINK_LIBRARIES(p2BTracker 
   MOOS
   MOOSGen
   opt
   m
   pthread)
ENDIF("${IVP_BUILD_p2BTracker}" STREQUAL "ON")
