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

if(CMAKE_SYSTEM_NAME STREQUAL Linux)
  SET(SYSTEM_LIBS ${SYSTEM_LIBS} GL)
endif(CMAKE_SYSTEM_NAME STREQUAL Linux)

SET(SRC
   LogPlotViewer.cpp
   NavPlotViewer.cpp
   REPLAY_GUI.cpp
   GUI_IPF.cpp
   GUI_HelmScope.cpp
   GUI_VarScope.cpp
   VPlugPlot.cpp
   VarPlot.cpp
   VarPlotEntry.cpp
   LogPlot.cpp
   Populator_VPlugPlots.cpp
   Populator_HelmPlots.cpp
   Populator_IPF_Plot.cpp
   IPF_Plot.cpp
   HelmPlot.cpp
   ModelHelmScope.cpp
   ModelVarScope.cpp
   IvPFuncViewerX.cpp
   LogViewLauncher.cpp
   ALogDataBroker.cpp
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
   helmivp
   geometry
   contacts
   ipfview
   logutils
   ivpbuild
   apputil
   mbutil
   ${MOOSGeodesy_LIBRARIES}
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
