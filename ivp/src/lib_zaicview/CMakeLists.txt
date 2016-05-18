#--------------------------------------------------------
# The CMakeLists.txt for:                    lib_zaicview
# Author(s):                                Mike Benjamin
#--------------------------------------------------------

SET(SRC
  ZAIC_Model.cpp
  ZAIC_Viewer.cpp
  ZAIC_GUI.cpp
)

SET(HEADERS
  ZAIC_Model.h
  ZAIC_Viewer.h
  ZAIC_GUI.h
)

# Build Library
ADD_LIBRARY(zaicview ${SRC})

