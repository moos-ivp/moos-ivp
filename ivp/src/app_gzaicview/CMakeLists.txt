#--------------------------------------------------------
# The CMakeLists.txt for:                   app_gzaicview
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

IF (${APPLE})
      # If you want to create an Apple BUNDLE, do it here
      ADD_EXECUTABLE(gzaicview ${SRC})
ELSE (${APPLE})
      ADD_EXECUTABLE(gzaicview ${SRC})
ENDIF (${APPLE})


TARGET_LINK_LIBRARIES(gzaicview 
  ${SYSTEM_LIBS}
  ivpbuild
  mbutil
  ivpcore
  ivpsolve
  fltk 
  fltk_gl
  dl
)

# http://developer.apple.com/qa/qa2007/qa1567.html
IF (${APPLE})
  SET_TARGET_PROPERTIES(gzaicview PROPERTIES	  
    LINK_FLAGS "-framework OpenGL -framework AGL -framework Cocoa"
    )
ENDIF (${APPLE})
