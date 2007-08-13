FILE(GLOB SRC *.cpp)

OPTION(IVP_BUILD_iMarineSim "Build program iMarineSim" ON)

IF("${IVP_BUILD_iMarineSim}" STREQUAL "ON")
   ADD_EXECUTABLE(iMarineSim ${SRC})
   
   TARGET_LINK_LIBRARIES(iMarineSim 
   MOOS
   MOOSGen
   mbutil
   geometry
   m
   pthread)
ENDIF("${IVP_BUILD_iMarineSim}" STREQUAL "ON")
