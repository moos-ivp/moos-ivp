#--------------------------------------------------------
# The CMakeLists.txt for:                    app_alogview
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
   LogPlotViewer.cpp
   NavPlotViewer.cpp
   IvPFuncViewer.cpp
   REPLAY_GUI.cpp
   LogViewLauncher.cpp
   main.cpp
)
   

IF (${APPLE})
      # If you want to create an Apple BUNDLE, do it here
      ADD_EXECUTABLE(alogview ${SRC})
ELSE (${APPLE})
      ADD_EXECUTABLE(alogview ${SRC})
ENDIF (${APPLE})
      
TARGET_LINK_LIBRARIES(alogview
   marineview
   geometry
   logutils
   ${MOOSGeodesy_LIBRARIES}
   MOOS
   contacts
   navplot
   ipfview
   mbutil
   ${SYSTEM_LIBS}
   
   # Standard libraries used by this project's FLTK apps...
   fltk 
   fltk_gl 
   dl
   tiff)
 
# http://developer.apple.com/qa/qa2007/qa1567.html
IF (${APPLE})
  SET_TARGET_PROPERTIES(alogview PROPERTIES	  
    LINK_FLAGS "-framework OpenGL -framework AGL -framework Cocoa"
    )
ENDIF (${APPLE})



