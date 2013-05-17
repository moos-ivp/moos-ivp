#--------------------------------------------------------
# The CMakeLists.txt for:                     app_cpaview
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
      ADD_EXECUTABLE(cpaview ${SRC})
ELSE (${APPLE})
      ADD_EXECUTABLE(cpaview ${SRC})
      # Install Targets
ENDIF (${APPLE})

TARGET_LINK_LIBRARIES(cpaview 
  marineview
  geometry
  ${MOOSGeodesy_LIBRARIES}
  contacts
  ipfview
  ivpbuild
  mbutil
  ivpcore
  fltk 
  fltk_gl 
  dl
  tiff
  ${SYSTEM_LIBS}
)
  
# http://developer.apple.com/qa/qa2007/qa1567.html
IF (${APPLE})
  SET_TARGET_PROPERTIES(cpaview PROPERTIES	  
    LINK_FLAGS "-framework OpenGL -framework AGL -framework Cocoa"
    )
ENDIF (${APPLE})

