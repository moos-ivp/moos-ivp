#-----------------------------------------------------------------------------
# The CMakeLists.txt for:                                       uMissionTester
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
  MissionTester.cpp
  MissionTesterThreadPool.cpp
  main_uMissionTester.cpp
  Process.cpp
  ProcessPosix.cpp
  ProcessWin.cpp
  StopWatch.cpp
)

SET(HDR
  MissionTester.h
  MissionTesterThreadPool.h
  Process.h
  ProcessPosix.h
  ProcessWin.h
  StopWatch.h
)

ADD_EXECUTABLE(uMissionTester ${SRC} ${HDR} )

TARGET_LINK_LIBRARIES(uMissionTester
  MOOS
  MOOSGen
  genutil
  mbutil
  logic
  ${SYSTEM_LIBS})

# Install Targets
INSTALL(TARGETS uMissionTester
  RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin
)

