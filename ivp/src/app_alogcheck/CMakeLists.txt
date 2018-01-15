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
  main.cpp
  LogChecker.cpp
  LogChecker_Info.cpp)

SET(HDR
  LogChecker.h
  LogChecker_Info.h)

ADD_EXECUTABLE(alogcheck ${SRC} ${HDR} )

TARGET_LINK_LIBRARIES(alogcheck
  mbutil
  logic
  logutils
  ${SYSTEM_LIBS})

# Install Targets
INSTALL(TARGETS alogcheck
  RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin
)

