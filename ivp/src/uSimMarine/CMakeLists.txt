#--------------------------------------------------------
# The CMakeLists.txt for:                      uSimMarine
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
   USM_MOOSApp.cpp
   USM_Model.cpp
   USM_Info.cpp
   SimEngine.cpp
   ThrustMap.cpp
   main.cpp
)

ADD_EXECUTABLE(uSimMarine ${SRC})

TARGET_LINK_LIBRARIES(uSimMarine 
  MOOS
  MOOSGen
  MOOSUtility
  contacts
  geometry 
  mbutil
  ${SYSTEM_LIBS})

# Install Targets
INSTALL(TARGETS uSimMarine
  RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin
)

