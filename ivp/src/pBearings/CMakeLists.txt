FILE(GLOB SRC *.cpp)

OPTION(IVP_BUILD_pBearings "Build program pBearings" ON)

IF("${IVP_BUILD_pBearings}" STREQUAL "ON")
   ADD_EXECUTABLE(pBearings ${SRC})
   
   TARGET_LINK_LIBRARIES(pBearings 
   MOOS
   MOOSGen
   opt
   newmat
   m
   pthread)
ENDIF("${IVP_BUILD_pBearings}" STREQUAL "ON")
