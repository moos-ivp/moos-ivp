OPTION(IVP_BUILD_ipfview "Build library ipfview" ON)

IF("${IVP_BUILD_ipfview}" STREQUAL "ON")
   FILE(GLOB SRC *.cpp *.c)
   ADD_LIBRARY(ipfview ${SRC})
ENDIF("${IVP_BUILD_ipfview}" STREQUAL "ON")
