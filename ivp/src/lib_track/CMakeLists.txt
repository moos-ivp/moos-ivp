OPTION(IVP_BUILD_opt "Build library opt" ON)

IF("${IVP_BUILD_opt}" STREQUAL "ON")
   FILE(GLOB SRC *.cpp *.c)
   ADD_LIBRARY(opt ${SRC})
ENDIF("${IVP_BUILD_opt}" STREQUAL "ON")
