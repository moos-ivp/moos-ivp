FILE(GLOB SRC *.cpp)

OPTION(IVP_BUILD_logparse "Build program logparse" ON)

IF("${IVP_BUILD_logparse}" STREQUAL "ON")
   ADD_EXECUTABLE(logparse ${SRC})
   
   TARGET_LINK_LIBRARIES(logparse 
   mbutil
   m)
ENDIF("${IVP_BUILD_logparse}" STREQUAL "ON")
