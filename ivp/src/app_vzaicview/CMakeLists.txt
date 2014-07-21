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

if(CMAKE_SYSTEM_NAME STREQUAL Linux)
  SET(SYSTEM_LIBS ${SYSTEM_LIBS} GL)
endif(CMAKE_SYSTEM_NAME STREQUAL Linux)

FILE(GLOB SRC *.cpp)


If (${APPLE})
      # If you want to create an Apple BUNDLE, do it here
      ADD_EXECUTABLE(vzaicview ${SRC})
ELSE (${APPLE})
      ADD_EXECUTABLE(vzaicview ${SRC})
ENDIF (${APPLE})

Target_LINK_LIBRARIES(vzaicview         
  ${SYSTEM_LIBS}
  ivpbuild
  ivpcore
  ivpsolve
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



