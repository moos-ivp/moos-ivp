#--------------------------------------------------------
# The CMakeLists.txt for:                        uCommand
# Author(s):                                Mike Benjamin
#--------------------------------------------------------

# Set System Specific Libraries
if (${WIN32})
  SET(SYSTEM_LIBS wsock32)
else (${WIN32})
  SET(SYSTEM_LIBS m)
endif (${WIN32})

SET(SRC
  UCMD_Info.cpp
  UCMD_MOOSApp.cpp
  main.cpp
)

IF (${APPLE})
      # If you want to create an Apple BUNDLE, do it here
      ADD_EXECUTABLE(uCommand ${SRC})
      # Install Targets
      INSTALL(TARGETS uCommand
        RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin
      )
ELSE (${APPLE})
      ADD_EXECUTABLE(uCommand ${SRC})
      # Install Targets
      INSTALL(TARGETS uCommand
         RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin
      )
ENDIF (${APPLE})


TARGET_LINK_LIBRARIES(uCommand
  ${MOOS_LIBRARIES}
  ucommand
  genutil
  apputil
  mbutil
  fltk 
)

# http://developer.apple.com/qa/qa2007/qa1567.html
IF (${APPLE})
  SET_TARGET_PROPERTIES(uCommand PROPERTIES	  
    LINK_FLAGS "-framework OpenGL -framework AGL -framework Cocoa"
    )
ENDIF (${APPLE})
