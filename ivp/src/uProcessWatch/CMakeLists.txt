FILE(GLOB SRC *.cpp)

OPTION(IVP_BUILD_uProcessWatch "Build program uProcessWatch" ON)

IF("${IVP_BUILD_uProcessWatch}" STREQUAL "ON")
   ADD_EXECUTABLE(uProcessWatch ${SRC})
   
   TARGET_LINK_LIBRARIES(uProcessWatch 
   MOOS
   MOOSGen
   mbutil
   m
   pthread)
ENDIF("${IVP_BUILD_uProcessWatch}" STREQUAL "ON")
