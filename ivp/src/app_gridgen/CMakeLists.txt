FILE(GLOB SRC *.cpp)

OPTION(IVP_BUILD_gridgen "Build program gridgen" ON)

IF("${IVP_BUILD_gridgen}" STREQUAL "ON")
   ADD_EXECUTABLE(gridgen ${SRC})
   
   TARGET_LINK_LIBRARIES(gridgen 
   ivpbuild
   ivpcore
   geometry
   mbutil
   m)
ENDIF("${IVP_BUILD_gridgen}" STREQUAL "ON")
