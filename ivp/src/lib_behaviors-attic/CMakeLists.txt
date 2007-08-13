OPTION(IVP_BUILD_behaviors-attic "Build library behaviors-attic" ON)

IF("${IVP_BUILD_behaviors-attic}" STREQUAL "ON")
   FILE(GLOB SRC *.cpp *.c)
   ADD_LIBRARY(behaviors-attic ${SRC})
ENDIF("${IVP_BUILD_behaviors-attic}" STREQUAL "ON")
