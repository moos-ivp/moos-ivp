#--------------------------------------------------------
# The CMakeLists.txt for:                    app_pairview
# Author(s):                                Mike Benjamin
#--------------------------------------------------------

# Set System Specific Libraries
if (${WIN32})
  SET(SYSTEM_LIBS wsock32)
else()
  SET(SYSTEM_LIBS m)
endif()

# Set GL Libraries
if (${APPLE})
  SET(GL_LIBS fltk_gl)
else ()
  SET(GL_LIBS fltk_gl GL)
endif ()

SET(SRC 
  PAIR_Viewer.cpp
  PAIR_GUI.cpp
  PairModel.cpp
  main.cpp
  )

ADD_EXECUTABLE(pairview ${SRC})

TARGET_LINK_LIBRARIES(pairview 
  marineview
  MOOSGeodesy
  logutils
  geometry
  contacts
  mbutil
  fltk 
  ${GL_LIBS}
  dl
  tiff
  ${SYSTEM_LIBS}
)
  
# http://developer.apple.com/qa/qa2007/qa1567.html
IF (${APPLE})
  SET_TARGET_PROPERTIES(pairview PROPERTIES	  
    LINK_FLAGS "-framework OpenGL -framework AGL -framework Cocoa"
    )
ENDIF (${APPLE})

