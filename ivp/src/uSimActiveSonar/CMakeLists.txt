#--------------------------------------------------------
# The CMakeLists.txt for:                 uSimActiveSonar
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
   SIMAS_MOOSApp.cpp
   SIMAS_Model.cpp
   SIMAS_ExampleConfig.cpp
   main.cpp
)

ADD_EXECUTABLE(uSimActiveSonar ${SRC})

TARGET_LINK_LIBRARIES(uSimActiveSonar
  MOOS
  MOOSGen
  MOOSUtility
  mbutil
  geometry 
  contacts
  ${SYSTEM_LIBS}
)

# Install Targets
INSTALL(TARGETS uSimActiveSonar
  RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin
)
