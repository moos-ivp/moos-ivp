#-----------------------------------------------------------------------------
# The CMakeLists.txt for:                                         app_alogtest
# Author(s):                                                     Mike Benjamin
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
  LogTest.cpp
  LogTester.cpp)

SET(HDR
  LogTester.h
  LogTest.h)


ADD_EXECUTABLE(alogtest ${SRC} ${HDR} )

TARGET_LINK_LIBRARIES(alogtest
  mbutil
  logic
  logutils
  ${SYSTEM_LIBS})

# Install Targets
INSTALL(TARGETS alogtest
  RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin
)

