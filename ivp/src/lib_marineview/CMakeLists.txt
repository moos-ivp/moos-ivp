OPTION(IVP_BUILD_marineview "Build library marineview" ON)

IF("${IVP_BUILD_marineview}" STREQUAL "ON")
   FILE(GLOB SRC *.cpp *.c)
   ADD_LIBRARY(marineview ${SRC})
   
   SET_TARGET_PROPERTIES(marineview
      PROPERTIES
      COMPILE_FLAGS -DDATA_DIR=\\"${IVP_DATA_DIR}\\"
      )
ENDIF("${IVP_BUILD_marineview}" STREQUAL "ON")
