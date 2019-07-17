#--------------------------------------------------------
# The CMakeLists.txt for:                        uMACView
# Author(s):                                Mike Benjamin
#--------------------------------------------------------

# Set System Specific Libraries
if (${WIN32})
  SET(SYSTEM_LIBS wsock32)
else (${WIN32})
  SET(SYSTEM_LIBS m)
endif (${WIN32})

FILE(GLOB SRC  *.cpp)

IF (${APPLE})
      # If you want to create an Apple BUNDLE, do it here
      ADD_EXECUTABLE(uMACView ${SRC})
      # Install Targets
      INSTALL(TARGETS uMACView
        RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin
      )
ELSE (${APPLE})
      ADD_EXECUTABLE(uMACView ${SRC})
      # Install Targets
      INSTALL(TARGETS uMACView
         RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin
      )
ENDIF (${APPLE})


TARGET_LINK_LIBRARIES(uMACView
  ${MOOS_LIBRARIES}
  genutil
  geometry
  apputil
  mbutil
  fltk 
  dl
)

# http://developer.apple.com/qa/qa2007/qa1567.html
IF (${APPLE})
  SET_TARGET_PROPERTIES(uMACView PROPERTIES	  
    LINK_FLAGS "-framework OpenGL -framework AGL -framework Cocoa"
    )
ENDIF (${APPLE})
