OPTION(IVP_BUILD_geometry "Build library geometry" ON)

IF("${IVP_BUILD_geometry}" STREQUAL "ON")
   FILE(GLOB SRC *.cpp *.c)
   ADD_LIBRARY(geometry ${SRC})
ENDIF("${IVP_BUILD_geometry}" STREQUAL "ON")
