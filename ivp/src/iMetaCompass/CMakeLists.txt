FILE(GLOB SRC *.cpp)

OPTION(IVP_BUILD_iMetaCompass "Build program iMetaCompass" ON)

IF("${IVP_BUILD_iMetaCompass}" STREQUAL "ON")
   ADD_EXECUTABLE(iMetaCompass ${SRC})
   
   TARGET_LINK_LIBRARIES(iMetaCompass 
   MOOS
   MOOSGen
   mbutil
   m
   pthread)
ENDIF("${IVP_BUILD_iMetaCompass}" STREQUAL "ON")
