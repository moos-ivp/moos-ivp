FILE(GLOB SRC *.cpp)

OPTION(IVP_BUILD_pTransponderAIS "Build program pTransponderAIS" ON)

IF("${IVP_BUILD_pTransponderAIS}" STREQUAL "ON")
   ADD_EXECUTABLE(pTransponderAIS ${SRC})
   
   TARGET_LINK_LIBRARIES(pTransponderAIS 
   MOOS
   MOOSGen
   mbutil
   m
   pthread)
ENDIF("${IVP_BUILD_pTransponderAIS}" STREQUAL "ON")
