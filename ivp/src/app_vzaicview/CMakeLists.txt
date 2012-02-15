#--------------------------------------------------------
# The CMakeLists.txt for:                   app_vzaicview
# Author(s):                                Mike Benjamin
#--------------------------------------------------------


# Set System Specific Libraries
if (${WIN32})
  SET(SYSTEM_LIBS wsock32)
else (${WIN32})
  SET(SYSTEM_LIBS m)
endif (${WIN32})

FILE(GLOB SRC *.cpp)


If (${APPLE})
      # If you want to create an Apple BUNDLE, do it here
      ADD_EXECUTABLE(vzaicview ${SRC})
      # Install Targets
      INSTALL(TARGETS vzaicview
        RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin
      )
ELSE (${APPLE})
      ADD_EXECUTABLE(vzaicview ${SRC})
      # Install Targets
      INSTALL(TARGETS vzaicview
         RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin
      )
ENDIF (${APPLE})

Target_LINK_LIBRARIES(vzaicview         
   ivpbuild
   ivpcore
   mbutil
      # Standard libraries used by this project's FLTK apps...
   fltk 
   fltk_gl 
   dl
   tiff)

# http://developer.apple.com/qa/qa2007/qa1567.html
IF (${APPLE})
  SET_TARGET_PROPERTIES(vzaicview PROPERTIES	  
    LINK_FLAGS "-framework OpenGL -framework AGL -framework Cocoa"
    )
ENDIF (${APPLE})



