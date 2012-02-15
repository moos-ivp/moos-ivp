#--------------------------------------------------------
# The CMakeLists.txt for:                        ivpcheck
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

SET(SRC main.cpp
	IvPChecker.cpp
	DomainExpander.cpp
	Populator_IPFBundleSeries.cpp
	IC_GUI.cpp
	IC_Viewer.cpp)


IF (${APPLE})
      # If you want to create an Apple BUNDLE, do it here
      ADD_EXECUTABLE(ivpcheck ${SRC})
      # Install Targets
      INSTALL(TARGETS ivpcheck
        RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin
      )
ELSE (${APPLE})
      ADD_EXECUTABLE(ivpcheck ${SRC})
      # Install Targets
      INSTALL(TARGETS ivpcheck
        RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin
      )
ENDIF (${APPLE})



   
TARGET_LINK_LIBRARIES(ivpcheck
  ipfview
  ivpbuild 
  ivpcore
  geometry
  mbutil
  logutils
  fltk 
  fltk_gl
  dl
  ${SYSTEM_LIBS})


# http://developer.apple.com/qa/qa2007/qa1567.html
IF (${APPLE})
  SET_TARGET_PROPERTIES(ivpcheck PROPERTIES	  
    LINK_FLAGS "-framework OpenGL -framework AGL -framework Cocoa"
    )
ENDIF (${APPLE})

