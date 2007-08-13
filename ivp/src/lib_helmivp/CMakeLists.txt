OPTION(IVP_BUILD_helmivp "Build library helmivp" ON)

IF("${IVP_BUILD_helmivp}" STREQUAL "ON")
   FILE(GLOB SRC *.cpp *.c)
   ADD_LIBRARY(helmivp ${SRC})
ENDIF("${IVP_BUILD_helmivp}" STREQUAL "ON")
