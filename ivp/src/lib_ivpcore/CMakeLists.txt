OPTION(IVP_BUILD_ivpcore "Build library ivpcore" ON)

IF("${IVP_BUILD_ivpcore}" STREQUAL "ON")
   FILE(GLOB SRC *.cpp *.c)
   ADD_LIBRARY(ivpcore ${SRC})
ENDIF("${IVP_BUILD_ivpcore}" STREQUAL "ON")
