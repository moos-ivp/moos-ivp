#--------------------------------------------------------
# The CMakeLists.txt for:                      pMarinePID
# Author(s):                                Mike Benjamin
#--------------------------------------------------------

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
   MarinePID.cpp
   PIDEngine.cpp
   ScalarPID.cpp
   PID_ExampleConfig.cpp
   main.cpp
)

ADD_EXECUTABLE(pMarinePID ${SRC})
   
TARGET_LINK_LIBRARIES(pMarinePID 
  MOOS
  MOOSGen
  mbutil
  geometry 
  ${SYSTEM_LIBS})

# Install Targets
INSTALL(TARGETS pMarinePID
  RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin
)
