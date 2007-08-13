FILE(GLOB SRC *.cpp)

OPTION(IVP_BUILD_pMarinePID "Build program pMarinePID" ON)

IF("${IVP_BUILD_pMarinePID}" STREQUAL "ON")
   ADD_EXECUTABLE(pMarinePID ${SRC})
   
   TARGET_LINK_LIBRARIES(pMarinePID 
   MOOS
   MOOSGen
   mbutil
   geometry
   m
   pthread)
ENDIF("${IVP_BUILD_pMarinePID}" STREQUAL "ON")
