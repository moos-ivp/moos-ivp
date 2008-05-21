#--------------------------------------------------------
# The CMakeLists.txt for:                         geoview
# Author(s):                                Mike Benjamin
#--------------------------------------------------------

IF(BUILD_GUIS)

   SET(SRC
      GEO_GUI.cpp
      GeoViewer.cpp
      main.cpp
   )

	  IF (${APPLE})
		 ADD_EXECUTABLE(geoview MACOSX_BUNDLE ${SRC})
	  ELSE (${APPLE})
		 ADD_EXECUTABLE(geoview ${SRC})
	  ENDIF (${APPLE})
         
   TARGET_LINK_LIBRARIES(geoview
      marineview
      mbutil
      geometry
      m
      
      # Standard libraries used by this project's FLTK apps...
      fltk 
      fltk_gl 
      tiff)

	  # http://developer.apple.com/qa/qa2007/qa1567.html
	  IF (${APPLE})
		  SET_TARGET_PROPERTIES(geoview PROPERTIES	  
		  	LINK_FLAGS "-Wl,-dylib_file,/System/Library/Frameworks/OpenGL.framework/Versions/A/Libraries/libGL.dylib:/System/Library/Frameworks/OpenGL.framework/Versions/A/Libraries/libGL.dylib"
		  )
	  ENDIF (${APPLE})

ENDIF(BUILD_GUIS)
