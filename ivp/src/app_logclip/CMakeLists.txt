FILE(GLOB SRC *.cpp)

OPTION(IVP_BUILD_logclip "Build program logclip" ON)

IF("${IVP_BUILD_logclip}" STREQUAL "ON")
   ADD_EXECUTABLE(logclip ${SRC})
   
   TARGET_LINK_LIBRARIES(logclip 
   mbutil
   m)
ENDIF("${IVP_BUILD_logclip}" STREQUAL "ON")
