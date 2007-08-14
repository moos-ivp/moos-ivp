OPTION(IVP_BUILD_newmat10D "Build library newmat10D" ON)

IF("${IVP_BUILD_newmat10D}" STREQUAL "ON")
   FILE(GLOB SRC *.cpp *.c)
   ADD_LIBRARY(newmat10D ${SRC})
ENDIF("${IVP_BUILD_newmat10D}" STREQUAL "ON")
