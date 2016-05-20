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

IF (${APPLE})
    SET_TARGET_PROPERTIES(zaicview
        PROPERTIES
        COMPILE_FLAGS ${CXXFLAGS}\ -F/System/Library/Frameworks/OpenGL.framework
        )

    ADD_DEFINITIONS(-DOPSYS_IS_OS_X=1)    
  ELSE (${APPLE})    
    ADD_DEFINITIONS(-DOPSYS_IS_LINUX=1)
ENDIF (${APPLE})
