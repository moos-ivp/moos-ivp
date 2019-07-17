#--------------------------------------------------------
# The CMakeLists.txt for:                      app_ffview
# Author(s):                                Mike Benjamin
#--------------------------------------------------------

# Set System Specific Libraries
if (${WIN32})
  SET(SYSTEM_LIBS wsock32)
else (${WIN32})
  SET(SYSTEM_LIBS m dl)
endif (${WIN32})

if(CMAKE_SYSTEM_NAME STREQUAL Linux)
  SET(SYSTEM_LIBS ${SYSTEM_LIBS} GL)
endif(CMAKE_SYSTEM_NAME STREQUAL Linux)

SET(SRC
  AOFFactory.cpp
  FFV_GUI.cpp
  FFV_Viewer.cpp
  PopulatorAOF.cpp
  main.cpp
  )

ADD_EXECUTABLE(ffview ${SRC})
	     
TARGET_LINK_LIBRARIES(ffview
     ipfview
     geometry
     mbutil
     genutil
     ivpbuild
     ivpcore
     ivpsolve
     ${SYSTEM_LIBS}
     # Standard libraries used by this project's FLTK apps...
     fltk 
     fltk_gl
) 

# http://developer.apple.com/qa/qa2007/qa1567.html
IF (${APPLE})
  SET_TARGET_PROPERTIES(ffview PROPERTIES	  
    LINK_FLAGS "-framework OpenGL -framework AGL -framework Cocoa"
    )
ENDIF (${APPLE})
