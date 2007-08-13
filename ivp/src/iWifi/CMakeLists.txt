FILE(GLOB SRC *.cpp)

OPTION(IVP_BUILD_iWifi "Build program iWifi" ON)

IF("${IVP_BUILD_iWifi}" STREQUAL "ON")
   ADD_EXECUTABLE(iWifi ${SRC})
   
   TARGET_LINK_LIBRARIES(iWifi 
   MOOS
   MOOSGen
   m
   pthread)
ENDIF("${IVP_BUILD_iWifi}" STREQUAL "ON")
