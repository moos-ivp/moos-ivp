FILE(GLOB SRC *.cpp)

OPTION(IVP_BUILD_pTarget "Build program pTarget" ON)

IF("${IVP_BUILD_pTarget}" STREQUAL "ON")
   ADD_EXECUTABLE(pTarget ${SRC})
   
   TARGET_LINK_LIBRARIES(pTarget 
   MOOS
   MOOSGen
   geometry
   mbutil
   m
   pthread)
ENDIF("${IVP_BUILD_pTarget}" STREQUAL "ON")
