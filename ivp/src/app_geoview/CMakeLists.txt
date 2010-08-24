#--------------------------------------------------------
# The CMakeLists.txt for:                         geoview
# Author(s):                                Mike Benjamin
#--------------------------------------------------------

# Set System Specific Libraries
if (${WIN32})
  SET(SYSTEM_LIBS
    wsock32)
else (${WIN32})
  SET(SYSTEM_LIBS
    m)
endif (${WIN32})

   SET(SRC
      GEO_GUI.cpp
      GeoViewer.cpp
      main.cpp
   )

	  IF (${APPLE})
     # If you want to create an Apple BUNDLE, do it here
		 ADD_EXECUTABLE(geoview ${SRC})
                 #Install Targets
                 INSTALL(TARGETS geoview
                   RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin
                 )
	  ELSE (${APPLE})
		 ADD_EXECUTABLE(geoview ${SRC})
                 #Install Targets
                 INSTALL(TARGETS geoview
                   RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin
                 )
	  ENDIF (${APPLE})
         
   TARGET_LINK_LIBRARIES(geoview
      marineview
      MOOSUtility
      mbutil
      geometry
      ${SYSTEM_LIBS}
      
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



