#--------------------------------------------------------
# The CMakeLists.txt for:                     uPlotViewer
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
      ADD_EXECUTABLE(uPlotViewer ${SRC})
      # Install Targets
      INSTALL(TARGETS uPlotViewer
        RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin
      )
ELSE (${APPLE})
      ADD_EXECUTABLE(uPlotViewer ${SRC})
      # Install Targets
      INSTALL(TARGETS uPlotViewer
         RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin
      )
ENDIF (${APPLE})


TARGET_LINK_LIBRARIES(uPlotViewer
  ${MOOS_LIBRARIES}
  genutil
  ivpbuild
  mbutil
  ivpcore
  fltk 
  fltk_gl
  dl
)

# http://developer.apple.com/qa/qa2007/qa1567.html
IF (${APPLE})
  SET_TARGET_PROPERTIES(uPlotViewer PROPERTIES	  
    LINK_FLAGS "-framework OpenGL -framework AGL -framework Cocoa"
    )
ENDIF (${APPLE})
