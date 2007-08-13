OPTION(IVP_BUILD_behaviors-marine "Build library behaviors-marine" ON)

IF("${IVP_BUILD_behaviors-marine}" STREQUAL "ON")
   FILE(GLOB SRC *.cpp *.c)
   ADD_LIBRARY(behaviors-marine ${SRC})
ENDIF("${IVP_BUILD_behaviors-marine}" STREQUAL "ON")
