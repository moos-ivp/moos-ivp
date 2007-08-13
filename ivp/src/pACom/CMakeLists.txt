FILE(GLOB SRC *.cpp)

OPTION(IVP_BUILD_pACom "Build program pACom" ON)

IF("${IVP_BUILD_pACom}" STREQUAL "ON")
   ADD_EXECUTABLE(pACom ${SRC})
   
   TARGET_LINK_LIBRARIES(pACom 
   MOOS
   MOOSGen
   MOOSUtility
   m
   pthread)
ENDIF("${IVP_BUILD_pACom}" STREQUAL "ON")
