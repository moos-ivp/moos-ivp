FILE(GLOB SRC *.cpp)

OPTION(IVP_BUILD_pSensorSim "Build program pSensorSim" ON)

IF("${IVP_BUILD_pSensorSim}" STREQUAL "ON")
   ADD_EXECUTABLE(pSensorSim ${SRC})
   
   TARGET_LINK_LIBRARIES(pSensorSim 
   MOOS
   MOOSGen
   mbutil
   m
   pthread
   artifacts
   geometry)
ENDIF("${IVP_BUILD_pSensorSim}" STREQUAL "ON")
