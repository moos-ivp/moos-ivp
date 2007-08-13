OPTION(IVP_BUILD_behaviors-colregs "Build library behaviors-colregs" ON)

IF("${IVP_BUILD_behaviors-colregs}" STREQUAL "ON")
   FILE(GLOB SRC *.cpp *.c)
   ADD_LIBRARY(behaviors-colregs ${SRC})
ENDIF("${IVP_BUILD_behaviors-colregs}" STREQUAL "ON")
