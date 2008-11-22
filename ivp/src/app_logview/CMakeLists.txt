#--------------------------------------------------------
# The CMakeLists.txt for:                     app_logview
# Author(s):                                Mike Benjamin
#--------------------------------------------------------

IF(BUILD_GUIS)

	SET(SRC
            LogPlotViewer.cpp
            NavPlotViewer.cpp
            REPLAY_GUI.cpp
            main.cpp
        )
	  
   
	  IF (${APPLE})
		 ADD_EXECUTABLE(logview MACOSX_BUNDLE ${SRC})
	  ELSE (${APPLE})
		 ADD_EXECUTABLE(logview ${SRC})
	  ENDIF (${APPLE})
	     
	TARGET_LINK_LIBRARIES(logview
	  marineview
	  MOOSUtility
	  navplot
	  geometry
	  mbutil
	  m

	  # Standard libraries used by this project's FLTK apps...
	  fltk 
	  fltk_gl 
	  tiff)
	  # http://developer.apple.com/qa/qa2007/qa1567.html
	  IF (${APPLE})
		  SET_TARGET_PROPERTIES(logview PROPERTIES	  
		  	LINK_FLAGS "-Wl,-dylib_file,/System/Library/Frameworks/OpenGL.framework/Versions/A/Libraries/libGL.dylib:/System/Library/Frameworks/OpenGL.framework/Versions/A/Libraries/libGL.dylib"
		  )
	  ENDIF (${APPLE})

	  
ENDIF(BUILD_GUIS)
