FILE(GLOB SRC *.cpp)

OPTION(IVP_BUILD_pExtTracker "Build program pExtTracker" ON)

IF("${IVP_BUILD_pExtTracker}" STREQUAL "ON")
   ADD_EXECUTABLE(pExtTracker ${SRC})
   
   TARGET_LINK_LIBRARIES(pExtTracker 
   MOOS
   MOOSGen
   m
   pthread)
ENDIF("${IVP_BUILD_pExtTracker}" STREQUAL "ON")
