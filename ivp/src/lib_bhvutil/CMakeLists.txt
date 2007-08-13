OPTION(IVP_BUILD_bhvutil "Build library bhvutil" ON)

IF("${IVP_BUILD_bhvutil}" STREQUAL "ON")
   FILE(GLOB SRC *.cpp *.c)
   ADD_LIBRARY(bhvutil ${SRC})
ENDIF("${IVP_BUILD_bhvutil}" STREQUAL "ON")
