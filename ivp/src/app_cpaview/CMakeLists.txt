#--------------------------------------------------------
# The CMakeLists.txt for:                     app_cpaview
# Author(s):                                Mike Benjamin
#--------------------------------------------------------

# Set System Specific Libraries
if (${WIN32})
  SET(SYSTEM_LIBS wsock32)
else (${WIN32})
  SET(SYSTEM_LIBS m)
endif (${WIN32})

FILE(GLOB SRC *.cpp)

IF (${APPLE})
      # If you want to create an Apple BUNDLE, do it here
      ADD_EXECUTABLE(cpaview ${SRC})
      # Install Targets
      INSTALL(TARGETS cpaview
        RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin
      )
ELSE (${APPLE})
      ADD_EXECUTABLE(cpaview ${SRC})
      # Install Targets
      INSTALL(TARGETS cpaview
         RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin
      )
ENDIF (${APPLE})

TARGET_LINK_LIBRARIES(cpaview 
  marineview
  contacts
  MOOSUtility
  ipfview
  geometry
  ivpbuild
  mbutil
  ivpcore
  fltk 
  fltk_gl 
  tiff
  ${SYSTEM_LIBS}
)
  
# http://developer.apple.com/qa/qa2007/qa1567.html
IF (${APPLE})
   SET_TARGET_PROPERTIES(cpaview PROPERTIES	  
   LINK_FLAGS "-Wl,-dylib_file,/System/Library/Frameworks/OpenGL.framework/Versions/A/Libraries/libGL.dylib:/System/Library/Frameworks/OpenGL.framework/Versions/A/Libraries/libGL.dylib"
   )
ENDIF (${APPLE})

