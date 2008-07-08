#--------------------------------------------------------
# The CMakeLists.txt for:                 pShipsideViewer
# Author(s):                                Mike Benjamin
#--------------------------------------------------------

IF(BUILD_GUIS)
   SET(SRC
       SSV_GUI.cpp
       SSV_MOOSApp.cpp
       SSV_Viewer.cpp
       main_ssviewer.cpp)
   
       IF(${APPLE})
         ADD_EXECUTABLE(pShipsideViewer MACOSX_BUNDLE ${SRC})
       ELSE(${APPLE})
         ADD_EXECUTABLE(pShipsideViewer ${SRC})
       ENDIF (${APPLE})
	     
       TARGET_LINK_LIBRARIES(pShipsideViewer
          MOOS
          MOOSGen
          marineview
          MOOSUtility
          mbutil
          genutil
          geometry
          m
          pthread
          GL
          fltk 
          fltk_gl 
          tiff)

	  # http://developer.apple.com/qa/qa2007/qa1567.html
	  IF(${APPLE})
            SET_TARGET_PROPERTIES(pShipsideViewer PROPERTIES	  
		  LINK_FLAGS "-Wl,-dylib_file,/System/Library/Frameworks/OpenGL.framework/Versions/A/Libraries/libGL.dylib:/System/Library/Frameworks/OpenGL.framework/Versions/A/Libraries/libGL.dylib"
		  )
	  ENDIF (${APPLE})
	   
ENDIF(BUILD_GUIS)

