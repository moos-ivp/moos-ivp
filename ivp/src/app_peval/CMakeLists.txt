FILE(GLOB SRC *.cpp)

OPTION(IVP_BUILD_peval "Build program peval" ON)

IF("${IVP_BUILD_peval}" STREQUAL "ON")
   ADD_EXECUTABLE(peval ${SRC})
   
   TARGET_LINK_LIBRARIES(peval 
   navplot
   mbutil
   geometry
   m)
ENDIF("${IVP_BUILD_peval}" STREQUAL "ON")
