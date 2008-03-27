#--------------------------------------------------------
# The CMakeLists.txt for:                  lib_ais
# Author(s):                               Phong Tran
#--------------------------------------------------------

FILE(GLOB SRC
 OM1371Message.cpp
 OM1371Message_1.cpp
 OM1371Message_5.cpp
 OM1371Message_18.cpp
 OM1371Message_19.cpp)


ADD_LIBRARY(ais ${SRC})
   
SET_TARGET_PROPERTIES(ais
    PROPERTIES
    COMPILE_FLAGS -DDATA_DIR=\\"${IVP_DATA_DIR}\\"
    )
