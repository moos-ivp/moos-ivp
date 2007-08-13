OPTION(IVP_BUILD_genutil "Build library genutil" ON)

IF("${IVP_BUILD_genutil}" STREQUAL "ON")
   FILE(GLOB SRC *.cpp *.c)
   ADD_LIBRARY(genutil ${SRC})
ENDIF("${IVP_BUILD_genutil}" STREQUAL "ON")
