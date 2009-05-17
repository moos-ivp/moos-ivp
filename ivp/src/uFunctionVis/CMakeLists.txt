#--------------------------------------------------------
# The CMakeLists.txt for:                    uFunctionVis
# Author(s):                                Mike Benjamin
#--------------------------------------------------------

SET(SRC
   FV_GUI.cpp
   FV_MOOSApp.cpp
   FV_Model.cpp
   FV_Viewer.cpp
   FunctionQueue.cpp
   QueueEntry.cpp
   uFunctionVisMain.cpp)

IF (${APPLE})
      ADD_EXECUTABLE(uFunctionVis MACOSX_BUNDLE ${SRC})
ELSE (${APPLE})
      ADD_EXECUTABLE(uFunctionVis ${SRC})
ENDIF (${APPLE})

TARGET_LINK_LIBRARIES(uFunctionVis
   MOOS
   MOOSGen
   pthread
   ipfview
   geometry
   ivpbuild
   mbutil
   ivpcore
   genutil
   m
   fltk 
   fltk_gl
   GL)

# http://developer.apple.com/qa/qa2007/qa1567.html
IF (${APPLE})
      SET_TARGET_PROPERTIES(uFunctionVis PROPERTIES	  
            LINK_FLAGS "-Wl,-dylib_file,/System/Library/Frameworks/OpenGL.framework/Versions/A/Libraries/libGL.dylib:/System/Library/Frameworks/OpenGL.framework/Versions/A/Libraries/libGL.dylib"
      )
ENDIF (${APPLE})
