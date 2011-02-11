#-----------------------------------------------------------------------------
# The CMakeLists.txt for:                                        app_alogcheck
# Author(s):                                                      Chris Gagner
#-----------------------------------------------------------------------------

# Set System Specific Libraries
if (${WIN32})
  SET(SYSTEM_LIBS
    wsock32)
else (${WIN32})
  SET(SYSTEM_LIBS
    m
    pthread)
endif (${WIN32})

SET(SRC 
   alogcheck_main.cpp
   LogChecker.cpp
)

SET(HDR
   LogChecker.h
)

ADD_EXECUTABLE(alogcheck ${SRC} ${HDR} )

TARGET_LINK_LIBRARIES(alogcheck
  MOOS
  MOOSGen
  genutil
  mbutil
  logic
  logutils
  ${SYSTEM_LIBS})

# Install Targets
INSTALL(TARGETS alogcheck
  RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin
)

