#--------------------------------------------------------
# The CMakeLists.txt for:                  lib_marineview
# Author(s):                                Mike Benjamin
#--------------------------------------------------------

SET(SRC
  BackImg.cpp
  MarineGUI.cpp
  MarineVehiGUI.cpp
  MarineViewer.cpp
  VMarkerSet.cpp
  OpAreaSpec.cpp
  LMV_Utils.cpp
  VPlug_GeoShapes.cpp
  VehicleSet.cpp
)


ADD_LIBRARY(marineview ${SRC})

ADD_DEFINITIONS(-DDATA_DIR=\\"${IVP_DATA_DIR}\\")

#SET(Z  ${CXXFLAGS}\ -F/System/Library/Frameworks/OpenGL.framework)
#message("Z = #${Z}#")


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

