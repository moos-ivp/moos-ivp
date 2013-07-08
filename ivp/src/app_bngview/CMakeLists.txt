#--------------------------------------------------------
# The CMakeLists.txt for:                     app_bngview
# Author(s):                                Mike Benjamin
#--------------------------------------------------------

# Set System Specific Libraries
if (${WIN32})
  SET(SYSTEM_LIBS wsock32)
else (${WIN32})
  SET(SYSTEM_LIBS m)
endif (${WIN32})

SET(SRC 
  BNGViewer.cpp
  BNG_GUI.cpp
  IPFViewer.cpp
  PairModel.cpp
  main.cpp
)

IF (${APPLE})
      # If you want to create an Apple BUNDLE, do it here
      ADD_EXECUTABLE(bngview ${SRC})
ELSE (${APPLE})
      ADD_EXECUTABLE(bngview ${SRC})
      # Install Targets
ENDIF (${APPLE})

TARGET_LINK_LIBRARIES(bngview 
  ${MOOSGeodesy_LIBRARIES}
  helmivp
  behaviors
  behaviors-marine
  bhvutil
  marineview
  genutil
  logic
  geometry
  contacts
  ipfview
  ivpbuild
  mbutil
  ivpcore
  fltk 
  fltk_gl 
  dl
  tiff
  ${SYSTEM_LIBS}
)
  
# http://developer.apple.com/qa/qa2007/qa1567.html
IF (${APPLE})
  SET_TARGET_PROPERTIES(bngview PROPERTIES	  
    LINK_FLAGS "-framework OpenGL -framework AGL -framework Cocoa"
    )
ENDIF (${APPLE})

