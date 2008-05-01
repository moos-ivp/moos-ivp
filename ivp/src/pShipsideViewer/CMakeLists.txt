#--------------------------------------------------------
# The CMakeLists.txt for:                 pShipsideViewer
# Author(s):                                Mike Benjamin
#--------------------------------------------------------

IF(BUILD_GUIS)

	FILE(GLOB SRC
	  SSV_GUI.cpp
	  SSV_MOOSApp.cpp
	  SSV_Viewer.cpp
	  main_ssviewer.cpp)
	  
	ADD_EXECUTABLE(pShipsideViewer ${SRC})
	     
	TARGET_LINK_LIBRARIES(pShipsideViewer
	   MOOS
	   MOOSGen
	   MOOSUtility
	   marineview
	   mbutil
	   genutil
	   geometry
	   m
	   pthread
	   GL
	   fltk 
	   fltk_gl 
	   tiff)
	   
ENDIF(BUILD_GUIS)

