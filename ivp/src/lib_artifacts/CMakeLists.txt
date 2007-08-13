OPTION(IVP_BUILD_artifacts "Build library artifacts" ON)

IF("${IVP_BUILD_artifacts}" STREQUAL "ON")
   FILE(GLOB SRC *.cpp *.c)
   ADD_LIBRARY(artifacts ${SRC})
ENDIF("${IVP_BUILD_artifacts}" STREQUAL "ON")
