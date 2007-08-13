FILE(GLOB SRC *.cpp)

OPTION(IVP_BUILD_pClusterManager "Build program pClusterManager" ON)

IF("${IVP_BUILD_pClusterManager}" STREQUAL "ON")
   ADD_EXECUTABLE(pClusterManager ${SRC})
   
   TARGET_LINK_LIBRARIES(pClusterManager 
   MOOS
   MOOSGen
   mbutil
   geometry
   m
   pthread)
ENDIF("${IVP_BUILD_pClusterManager}" STREQUAL "ON")
