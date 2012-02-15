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
      # Install Targets
      INSTALL(TARGETS uLogView
        RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin
      )
ELSE (${APPLE})
      ADD_EXECUTABLE(uLogView ${SRC})
      # Install Targets
      INSTALL(TARGETS uLogView
         RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin
      )
ENDIF (${APPLE})
      
Target_LINK_LIBRARIES(uLogView         
   MOOS
   MOOSGen
   ulogview
   marineview
   contacts
   MOOSUtility
   logutils
   genutil
   geometry
   contacts
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

ADD_TEST(MyTest1 ${EXECUTABLE_OUTPUT_PATH}/uLogView --version)

