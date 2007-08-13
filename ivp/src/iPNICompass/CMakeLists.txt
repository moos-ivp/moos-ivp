FILE(GLOB SRC *.cpp)

OPTION(IVP_BUILD_iPNICompass "Build program iPNICompass" ON)

IF("${IVP_BUILD_iPNICompass}" STREQUAL "ON")
   ADD_EXECUTABLE(iPNICompass ${SRC})
   
   TARGET_LINK_LIBRARIES(iPNICompass 
   MOOS
   MOOSGen
   MOOSUtility
   anrp_util
   m
   pthread)
ENDIF("${IVP_BUILD_iPNICompass}" STREQUAL "ON")
