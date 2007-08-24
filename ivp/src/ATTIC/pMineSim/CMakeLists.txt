FILE(GLOB SRC *.cpp)

OPTION(IVP_BUILD_pMineSim "Build program pMineSim" ON)

IF("${IVP_BUILD_pMineSim}" STREQUAL "ON")
   ADD_EXECUTABLE(pMineSim ${SRC})
   
   TARGET_LINK_LIBRARIES(pMineSim 
   MOOS
   MOOSGen
   geometry
   mbutil
   m
   pthread)
ENDIF("${IVP_BUILD_pMineSim}" STREQUAL "ON")
