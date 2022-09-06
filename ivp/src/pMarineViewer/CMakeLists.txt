#--------------------------------------------------------
# The CMakeLists.txt for:                   pMarineViewer
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
      ADD_EXECUTABLE(pMarineViewer ${SRC})
ELSE (${APPLE})
      ADD_EXECUTABLE(pMarineViewer ${SRC})
ENDIF (${APPLE})
      
TARGET_LINK_LIBRARIES(pMarineViewer         
  ${MOOS_LIBRARIES}
  ${MOOSGeodesy_LIBRARIES}
  ${SYSTEM_LIBS}
  genutil
  marineview
  contacts
  geometry
  ucommand
  realm
  apputil
  mbutil
  bhvutil
  # Standard libraries used by this project's FLTK apps...
  fltk 
  fltk_gl 
  dl
  tiff)

# http://developer.apple.com/qa/qa2007/qa1567.html
IF (${APPLE})
  SET_TARGET_PROPERTIES(pMarineViewer PROPERTIES	  
    LINK_FLAGS "-framework OpenGL -framework AGL -framework Cocoa"
    )
ENDIF (${APPLE})

ADD_TEST(MyTest1 ${EXECUTABLE_OUTPUT_PATH}/pMarineViewer --version)

