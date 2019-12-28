#--------------------------------------------------------
# The CMakeLists.txt for:                  lib_marineview
# Author(s):                                Mike Benjamin
#--------------------------------------------------------

SET(SRC
  BackImg.cpp
  MarineGUI.cpp
  MarineVehiGUI.cpp
  MarineViewer.cpp
  OpAreaSpec.cpp
  LMV_Utils.cpp
  VehicleSet.cpp
)


# This is used to supply a preprocessor definition that gets compiled into some
# code, such as lib_marineview.  I really dislike this approach because it's 
# very brittle w.r.t. anything getting moved around, or if you use the code on
# a different system than that on which it was compiled. -CJC
GET_FILENAME_COMPONENT(IVP_DATA_DIR ../../data ABSOLUTE)
GET_FILENAME_COMPONENT(IVP_DATA_DIRX ../../datax ABSOLUTE)

SET(ESCAPED_DATA_DIR  \\"${IVP_DATA_DIR}\\")
SET(ESCAPED_DATA_DIRX \\"${IVP_DATA_DIRX}\\")

SET(CMAKE_CXX_FLAGS "-DDATA_DIR=${ESCAPED_DATA_DIR} ${CMAKE_CXX_FLAGS}")
SET(CMAKE_CXX_FLAGS "-DDATA_DIRX=${ESCAPED_DATA_DIRX} ${CMAKE_CXX_FLAGS}")

ADD_LIBRARY(marineview ${SRC})

# http://developer.apple.com/qa/qa2007/qa1567.html
IF (${APPLE})
    SET_TARGET_PROPERTIES(marineview
        PROPERTIES
        COMPILE_FLAGS ${CXXFLAGS}\ -F/System/Library/Frameworks/OpenGL.framework
        )

    ADD_DEFINITIONS(-DOPSYS_IS_OS_X=1)    
ELSE (${APPLE})	    
    ADD_DEFINITIONS(-DOPSYS_IS_LINUX=1)
ENDIF (${APPLE})

#MESSAGE(FATAL_ERROR FOO_DATA_DIR ${FOO_DATA_DIR})
