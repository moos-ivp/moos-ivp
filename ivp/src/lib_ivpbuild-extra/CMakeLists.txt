OPTION(IVP_BUILD_ivpbuild-extra "Build library ivpbuild-extra" ON)

IF("${IVP_BUILD_ivpbuild-extra}" STREQUAL "ON")
   FILE(GLOB SRC *.cpp *.c)
   ADD_LIBRARY(ivpbuild-extra ${SRC})
ENDIF("${IVP_BUILD_ivpbuild-extra}" STREQUAL "ON")
