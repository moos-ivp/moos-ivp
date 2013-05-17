#--------------------------------------------------------
# The CMakeLists.txt for:                     uLogViewIPF
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

SET(SRC	LauncherULI.cpp
	ULI_MOOSApp.cpp
	ULI_GUI.cpp
	ULI_Info.cpp
	IvPFuncViewerX.cpp
	main.cpp
)

If (${APPLE})
      # If you want to create an Apple BUNDLE, do it here
      ADD_EXECUTABLE(uLogViewIPF ${SRC})
ELSE (${APPLE})
      ADD_EXECUTABLE(uLogViewIPF ${SRC})
ENDIF (${APPLE})
      
Target_LINK_LIBRARIES(uLogViewIPF         
  ${MOOS_LIBRARIES}
  ${SYSTEM_LIBS}
  marineview
  ipfview
  contacts
  ulogview
  logutils
  genutil
  geometry
  contacts
  mbutil
  # Standard libraries used by this project's FLTK apps...
  fltk 
  fltk_gl 
  dl
  tiff)

# http://developer.apple.com/qa/qa2007/qa1567.html
IF (${APPLE})
      SET_TARGET_PROPERTIES(uLogViewIPF PROPERTIES	  
         LINK_FLAGS "-framework OpenGL -framework AGL -framework Cocoa"
      )
ENDIF (${APPLE})

ADD_TEST(MyTest1 ${EXECUTABLE_OUTPUT_PATH}/uLogViewIPF --version)

