OPTION(IVP_BUILD_behaviors "Build library behaviors" ON)

IF("${IVP_BUILD_behaviors}" STREQUAL "ON")
   FILE(GLOB SRC *.cpp *.c)
   ADD_LIBRARY(behaviors ${SRC})
ENDIF("${IVP_BUILD_behaviors}" STREQUAL "ON")
