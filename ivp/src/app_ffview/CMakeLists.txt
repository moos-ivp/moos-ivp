#--------------------------------------------------------
# The CMakeLists.txt for:                      app_ffview
# Author(s):                                Mike Benjamin
#--------------------------------------------------------

# Set System Specific Libraries
if (${WIN32})
  SET(SYSTEM_LIBS wsock32)
else (${WIN32})
  SET(SYSTEM_LIBS m dl)
endif (${WIN32})

FILE(GLOB SRC *.cpp)

IF (${APPLE})
     # If you want to create an Apple BUNDLE, do it here
     ADD_EXECUTABLE(ffview ${SRC})
     #Install Targets
     INSTALL(TARGETS ffview
       RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin
     )

ELSE (${APPLE})
     ADD_EXECUTABLE(ffview ${SRC})
     #Install Targets
     INSTALL(TARGETS ffview
        RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin
     )
ENDIF (${APPLE})
	     
TARGET_LINK_LIBRARIES(ffview
     ipfview
     geometry
     mbutil
     genutil
     ivpbuild
     ivpcore
     ${SYSTEM_LIBS}
     # Standard libraries used by this project's FLTK apps...
     fltk 
     fltk_gl
) 

# http://developer.apple.com/qa/qa2007/qa1567.html
IF (${APPLE})
     SET_TARGET_PROPERTIES(ffview PROPERTIES	  
     LINK_FLAGS "-Wl,-dylib_file,/System/Library/Frameworks/OpenGL.framework/Versions/A/Libraries/libGL.dylib:/System/Library/Frameworks/OpenGL.framework/Versions/A/Libraries/libGL.dylib"
		  )
ENDIF (${APPLE})
