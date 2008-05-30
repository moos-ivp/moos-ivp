IF(BUILD_GUIS)

   SET(SRC
	main.cpp 
	WireframeViewer.cpp 
	GUI.cpp 
	circle.cpp 
	point.cpp 
	ray.cpp 
	arc.cpp 
	kayak.cpp
   )

	  IF (${APPLE})
		 ADD_EXECUTABLE(wireframe_viewer MACOSX_BUNDLE ${SRC})
	  ELSE (${APPLE})
		 ADD_EXECUTABLE(wireframe_viewer ${SRC})
	  ENDIF (${APPLE})
         
   TARGET_LINK_LIBRARIES(wireframe_viewer
      GL 
      glut 
      fltk 
      fltk_gl
   )

   # http://developer.apple.com/qa/qa2007/qa1567.html
   IF (${APPLE})
         SET_TARGET_PROPERTIES(wireframe_viewer PROPERTIES	  
               LINK_FLAGS "-Wl,-dylib_file,/System/Library/Frameworks/OpenGL.framework/Versions/A/Libraries/libGL.dylib:/System/Library/Frameworks/OpenGL.framework/Versions/A/Libraries/libGL.dylib"
         )
   ENDIF (${APPLE})

ENDIF(BUILD_GUIS)
