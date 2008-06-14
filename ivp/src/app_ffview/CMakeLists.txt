#--------------------------------------------------------
# The CMakeLists.txt for:                      app_ffview
# Author(s):                                Mike Benjamin
#--------------------------------------------------------

IF(BUILD_GUIS)

   SET(SRC
      AOF_Cache.cpp
      GUI.cpp
      PopulatorAOF.cpp
      Viewer.cpp
      main.cpp
   )	  

   
   IF (${APPLE})
     ADD_EXECUTABLE(ffview MACOSX_BUNDLE ${SRC})
   ELSE (${APPLE})
     ADD_EXECUTABLE(ffview ${SRC})
   ENDIF (${APPLE})
	     
   TARGET_LINK_LIBRARIES(ffview
     bhvutil
     ipfview
     behaviors
     geometry
     mbutil
     ivpbuild
     ivpcore
     m
	   
     # Standard libraries used by this project's FLTK apps...
     fltk 
     fltk_gl 
     tiff)

     # http://developer.apple.com/qa/qa2007/qa1567.html
     IF (${APPLE})
        SET_TARGET_PROPERTIES(ffview PROPERTIES	  
        LINK_FLAGS "-Wl,-dylib_file,/System/Library/Frameworks/OpenGL.framework/Versions/A/Libraries/libGL.dylib:/System/Library/Frameworks/OpenGL.framework/Versions/A/Libraries/libGL.dylib"
		  )
    ENDIF (${APPLE})
	  
ENDIF(BUILD_GUIS)
