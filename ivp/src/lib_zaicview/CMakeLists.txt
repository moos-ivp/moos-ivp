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
IVP_ADD_LIBRARY(zaicview ${SRC})
TARGET_LINK_LIBRARIES(zaicview
  PUBLIC ivpcore
    "$<BUILD_INTERFACE:ivp_fltk>$<INSTALL_INTERFACE:IVP::FLTK>"
  PRIVATE geometry ivpbuild mbutil)

IF (${APPLE})
    SET_TARGET_PROPERTIES(zaicview
        PROPERTIES
        COMPILE_FLAGS ${CXXFLAGS}\ -F/System/Library/Frameworks/OpenGL.framework
        )

    ADD_DEFINITIONS(-DOPSYS_IS_OS_X=1)    
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-cast-function-type ")
  ELSE (${APPLE})    
    ADD_DEFINITIONS(-DOPSYS_IS_LINUX=1)
ENDIF (${APPLE})
