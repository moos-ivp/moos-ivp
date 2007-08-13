FILE(GLOB SRC *.cpp)

OPTION(IVP_BUILD_tupelize "Build program tupelize" ON)

IF("${IVP_BUILD_tupelize}" STREQUAL "ON")
   ADD_EXECUTABLE(tupelize ${SRC})
   
   TARGET_LINK_LIBRARIES(tupelize 
   mbutil
   ivpbuild
   ivpcore
   m)
ENDIF("${IVP_BUILD_tupelize}" STREQUAL "ON")
