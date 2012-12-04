#--------------------------------------------------------
# The CMakeLists.txt for:                        uLogView
# Author(s):                                Mike Benjamin
#--------------------------------------------------------

# Set System Specific Libraries
if (${WIN32})
  SET(SYSTEM_LIBS wsock32)
else (${WIN32})
  SET(SYSTEM_LIBS m)
endif (${WIN32})

SET(SRC	LogPlotViewer.cpp
	NavPlotViewer.cpp
	NavPlotElement.cpp
	LauncherULV.cpp
	ULV_GUI.cpp
	ULV_MOOSApp.cpp
	main.cpp
)

If (${APPLE})
      # If you want to create an Apple BUNDLE, do it here
      ADD_EXECUTABLE(uLogView ${SRC})
ELSE (${APPLE})
      ADD_EXECUTABLE(uLogView ${SRC})
ENDIF (${APPLE})
      
TARGET_LINK_LIBRARIES(uLogView         
   ulogview
   marineview
   geometry
   logutils
   ${MOOSGeodesy_LIBRARIES}
   ${MOOS_LIBRARIES}
   genutil
   contacts
   apputil
   mbutil
      # Standard libraries used by this project's FLTK apps...
   fltk 
   fltk_gl 
   dl
   tiff)

# http://developer.apple.com/qa/qa2007/qa1567.html
IF (${APPLE})
      SET_TARGET_PROPERTIES(uLogView PROPERTIES	  
         LINK_FLAGS "-framework OpenGL -framework AGL -framework Cocoa"
      )
ENDIF (${APPLE})


