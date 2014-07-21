#--------------------------------------------------------
# The CMakeLists.txt for:                    uLogViewHelm
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

SET(SRC	LauncherULH.cpp
        HelmScopeModel.cpp
	ULH_MOOSApp.cpp
	ULH_GUI.cpp
	ULH_Info.cpp
	PopulatorHSM.cpp
	main.cpp
)

If (${APPLE})
      # If you want to create an Apple BUNDLE, do it here
      ADD_EXECUTABLE(uLogViewHelm ${SRC})
ELSE (${APPLE})
      ADD_EXECUTABLE(uLogViewHelm ${SRC})
ENDIF (${APPLE})
      
TARGET_LINK_LIBRARIES(uLogViewHelm         
  ${MOOS_LIBRARIES}
  ${SYSTEM_LIBS}
  contacts
  ulogview
  logutils
  helmivp
  genutil
  geometry
  mbutil
  # Standard libraries used by this project's FLTK apps...
  fltk 
  fltk_gl 
  dl
  tiff)

# http://developer.apple.com/qa/qa2007/qa1567.html
IF (${APPLE})
      SET_TARGET_PROPERTIES(uLogViewHelm PROPERTIES	  
         LINK_FLAGS "-framework OpenGL -framework AGL -framework Cocoa"
      )
ENDIF (${APPLE})

ADD_TEST(MyTest1 ${EXECUTABLE_OUTPUT_PATH}/uLogViewIPF --version)

