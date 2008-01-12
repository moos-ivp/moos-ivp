#--------------------------------------------------------
# The CMakeLists.txt for:                  lib_marineview
# Author(s):                                Mike Benjamin
#--------------------------------------------------------

FILE(GLOB SRC *.cpp *.c)
ADD_LIBRARY(marineview ${SRC})
   
SET_TARGET_PROPERTIES(marineview
    PROPERTIES
    COMPILE_FLAGS -DDATA_DIR=\\"${IVP_DATA_DIR}\\"
    )
