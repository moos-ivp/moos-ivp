OPTION(IVP_BUILD_behaviors-don "Build library behaviors-don" ON)

IF("${IVP_BUILD_behaviors-don}" STREQUAL "ON")
   FILE(GLOB SRC *.cpp *.c)
   ADD_LIBRARY(behaviors-don ${SRC})
ENDIF("${IVP_BUILD_behaviors-don}" STREQUAL "ON")
