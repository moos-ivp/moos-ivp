FILE(GLOB SRC *.cpp)

OPTION(IVP_BUILD_p1BTracker "Build program p1BTracker" ON)

IF("${IVP_BUILD_p1BTracker}" STREQUAL "ON")
   ADD_EXECUTABLE(p1BTracker ${SRC})
   
   TARGET_LINK_LIBRARIES(p1BTracker 
   MOOS
   MOOSGen
   opt
   m
   pthread)
ENDIF("${IVP_BUILD_p1BTracker}" STREQUAL "ON")
