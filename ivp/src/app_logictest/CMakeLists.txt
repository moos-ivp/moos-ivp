FILE(GLOB SRC *.cpp)

OPTION(IVP_BUILD_ltest "Build program ltest" ON)

IF("${IVP_BUILD_ltest}" STREQUAL "ON")
   ADD_EXECUTABLE(ltest ${SRC})
   
   TARGET_LINK_LIBRARIES(ltest 
   logic
   mbutil
   m)
ENDIF("${IVP_BUILD_ltest}" STREQUAL "ON")
