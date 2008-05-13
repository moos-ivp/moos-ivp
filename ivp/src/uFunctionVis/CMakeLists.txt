#--------------------------------------------------------
# The CMakeLists.txt for:                    uFunctionVis
# Author(s):                                Mike Benjamin
#--------------------------------------------------------

IF(BUILD_GUIS)

	SET(SRC
	  FV_GUI.cpp
	  FV_MOOSApp.cpp
	  FV_Model.cpp
	  FV_Viewer.cpp
	  FunctionQueue.cpp
	  QueueEntry.cpp
	  uFunctionVisMain.cpp)

	ADD_EXECUTABLE(uFunctionVis ${SRC})
	      
	TARGET_LINK_LIBRARIES(uFunctionVis
	  MOOS
	  MOOSGen
	  pthread
	  ipfview
	  geometry
	  ivpbuild
	  mbutil
	  ivpcore
	  genutil
	  m
	  fltk 
	  fltk_gl
	  GL)
  
ENDIF(BUILD_GUIS)
