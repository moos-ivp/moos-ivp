OPTION(IVP_BUILD_behaviors-oxford "Build library behaviors-oxford" ON)

IF("${IVP_BUILD_behaviors-oxford}" STREQUAL "ON")
   FILE(GLOB SRC *.cpp *.c)
   ADD_LIBRARY(behaviors-oxford ${SRC})
ENDIF("${IVP_BUILD_behaviors-oxford}" STREQUAL "ON")
