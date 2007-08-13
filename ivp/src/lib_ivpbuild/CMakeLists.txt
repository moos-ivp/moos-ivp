OPTION(IVP_BUILD_ivpbuild "Build library ivpbuild" ON)

IF("${IVP_BUILD_ivpbuild}" STREQUAL "ON")
   FILE(GLOB SRC *.cpp *.c)
   ADD_LIBRARY(ivpbuild ${SRC})
ENDIF("${IVP_BUILD_ivpbuild}" STREQUAL "ON")
