OPTION(IVP_BUILD_behaviors-sandbox "Build library behaviors-sandbox" ON)

IF("${IVP_BUILD_behaviors-sandbox}" STREQUAL "ON")
   FILE(GLOB SRC *.cpp *.c)
   ADD_LIBRARY(behaviors-sandbox ${SRC})
ENDIF("${IVP_BUILD_behaviors-sandbox}" STREQUAL "ON")
