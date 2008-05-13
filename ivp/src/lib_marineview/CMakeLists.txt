#--------------------------------------------------------
# The CMakeLists.txt for:                  lib_marineview
# Author(s):                                Mike Benjamin
#--------------------------------------------------------

SET(SRC
  BackImg.cpp
  MarineGUI.cpp
  MarineVehiGUI.cpp
  MarineViewer.cpp
)


ADD_LIBRARY(marineview ${SRC})
   
SET_TARGET_PROPERTIES(marineview
    PROPERTIES
    COMPILE_FLAGS -DDATA_DIR=\\"${IVP_DATA_DIR}\\"
    )
