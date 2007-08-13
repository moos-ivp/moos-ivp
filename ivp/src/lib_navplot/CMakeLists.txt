OPTION(IVP_BUILD_navplot "Build library navplot" ON)

IF("${IVP_BUILD_navplot}" STREQUAL "ON")
   FILE(GLOB SRC *.cpp *.c)
   ADD_LIBRARY(navplot ${SRC})
ENDIF("${IVP_BUILD_navplot}" STREQUAL "ON")
