FILE(GLOB SRC *.cpp)

OPTION(IVP_BUILD_pBearingsSim "Build program pBearingsSim" ON)

IF("${IVP_BUILD_pBearingsSim}" STREQUAL "ON")
   ADD_EXECUTABLE(pBearingsSim ${SRC})
   
   TARGET_LINK_LIBRARIES(pBearingsSim 
   MOOS
   MOOSGen
   opt
   m
   pthread)
ENDIF("${IVP_BUILD_pBearingsSim}" STREQUAL "ON")
