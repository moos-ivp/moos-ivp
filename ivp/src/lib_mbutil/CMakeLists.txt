OPTION(IVP_BUILD_mbutil "Build library mbutil" ON)

IF("${IVP_BUILD_mbutil}" STREQUAL "ON")
   FILE(GLOB SRC *.cpp *.c)
   ADD_LIBRARY(mbutil ${SRC})
ENDIF("${IVP_BUILD_mbutil}" STREQUAL "ON")
