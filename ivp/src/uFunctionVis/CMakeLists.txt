#--------------------------------------------------------
# The CMakeLists.txt for:                    uFunctionVis
# Author(s):                                Mike Benjamin
#--------------------------------------------------------

# Set System Specific Libraries
if (${WIN32})
  SET(SYSTEM_LIBS
    wsock32)
else (${WIN32})
  SET(SYSTEM_LIBS
    m
    pthread)
endif (${WIN32})

SET(SRC
   FV_GUI.cpp
   FV_MOOSApp.cpp
   FV_Model.cpp
   FV_Viewer.cpp
   FunctionQueue.cpp
   QueueEntry.cpp
   uFunctionVisMain.cpp)

IF (${APPLE})
      # If you want to create an Apple BUNDLE, do it here
      ADD_EXECUTABLE(uFunctionVis ${SRC})
      # Install Targets
      INSTALL(TARGETS uFunctionVis
        RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin
      )
ELSE (${APPLE})
      ADD_EXECUTABLE(uFunctionVis ${SRC})
      # Install Targets
      INSTALL(TARGETS uFunctionVis
        RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin
      )
ENDIF (${APPLE})

TARGET_LINK_LIBRARIES(uFunctionVis
   MOOS
   MOOSGen
   ipfview
   geometry
   ivpbuild
   mbutil
   ivpcore
   genutil
   fltk 
   fltk_gl
   ${SYSTEM_LIBS})

# http://developer.apple.com/qa/qa2007/qa1567.html
IF (${APPLE})
      SET_TARGET_PROPERTIES(uFunctionVis PROPERTIES	  
            LINK_FLAGS "-Wl,-dylib_file,/System/Library/Frameworks/OpenGL.framework/Versions/A/Libraries/libGL.dylib:/System/Library/Frameworks/OpenGL.framework/Versions/A/Libraries/libGL.dylib"
      )
ENDIF (${APPLE})

