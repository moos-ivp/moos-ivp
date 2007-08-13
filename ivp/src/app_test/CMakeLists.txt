FILE(GLOB SRC *.cpp)

OPTION(IVP_BUILD_stest "Build program stest" ON)

IF("${IVP_BUILD_stest}" STREQUAL "ON")
   ADD_EXECUTABLE(stest ${SRC})
   
   TARGET_LINK_LIBRARIES(stest 
   ivpbuild
   mbutil
   geometry
   ivpcore
   logic
   m)
ENDIF("${IVP_BUILD_stest}" STREQUAL "ON")
