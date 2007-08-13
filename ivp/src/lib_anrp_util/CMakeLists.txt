OPTION(IVP_BUILD_anrp_util "Build library anrp_util" ON)

IF("${IVP_BUILD_anrp_util}" STREQUAL "ON")
   FILE(GLOB SRC *.cpp *.c)
   ADD_LIBRARY(anrp_util ${SRC})
ENDIF("${IVP_BUILD_anrp_util}" STREQUAL "ON")
