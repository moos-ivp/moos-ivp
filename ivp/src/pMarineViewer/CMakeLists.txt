#--------------------------------------------------------
# The CMakeLists.txt for:                   pMarineViewer
# Author(s):                                Mike Benjamin
#--------------------------------------------------------

IF(BUILD_GUIS)

   SET(SRC
      PMV_GUI.cpp
      PMV_MOOSApp.cpp
      PMV_Viewer.cpp
      main_mviewer.cpp
   )

	  IF (${APPLE})
		 ADD_EXECUTABLE(pMarineViewer MACOSX_BUNDLE ${SRC})
	  ELSE (${APPLE})
		 ADD_EXECUTABLE(pMarineViewer ${SRC})
	  ENDIF (${APPLE})
         
   TARGET_LINK_LIBRARIES(pMarineViewer         
      MOOS
      MOOSGen
      marineview
      MOOSUtility
      mbutil
      genutil
      geometry
      m
      
      # Standard libraries used by this project's FLTK apps...
      fltk 
      fltk_gl 
      tiff)

	  # http://developer.apple.com/qa/qa2007/qa1567.html
	  IF (${APPLE})
		  SET_TARGET_PROPERTIES(pMarineViewer PROPERTIES	  
		  	LINK_FLAGS "-Wl,-dylib_file,/System/Library/Frameworks/OpenGL.framework/Versions/A/Libraries/libGL.dylib:/System/Library/Frameworks/OpenGL.framework/Versions/A/Libraries/libGL.dylib"
		  )
	  ENDIF (${APPLE})
  
ENDIF(BUILD_GUIS)
