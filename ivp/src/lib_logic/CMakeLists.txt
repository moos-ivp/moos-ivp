OPTION(IVP_BUILD_logic "Build library logic" ON)

IF("${IVP_BUILD_logic}" STREQUAL "ON")
   FILE(GLOB SRC *.cpp *.c)
   ADD_LIBRARY(logic ${SRC})
ENDIF("${IVP_BUILD_logic}" STREQUAL "ON")
