#--------------------------------------------------------
# The CMakeLists.txt for:                    app_zaic_hdg
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

SET(SRC
  main.cpp
  ZAIC_HDG_GUI.cpp
  ZAIC_HDG_Model.cpp
)

ADD_EXECUTABLE(zaic_hdg ${SRC})

TARGET_LINK_LIBRARIES(zaic_hdg
  zaicview
  ivpbuild
  mbutil
  fltk 
  fltk_gl
  zaicview
  dl
  ${SYSTEM_LIBS}
)

# http://developer.apple.com/qa/qa2007/qa1567.html
IF (${APPLE})
  SET_TARGET_PROPERTIES(zaic_hdg PROPERTIES	  
    LINK_FLAGS "-framework OpenGL -framework AGL -framework Cocoa"
    )
ENDIF (${APPLE})

