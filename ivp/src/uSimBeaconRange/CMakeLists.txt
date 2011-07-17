#--------------------------------------------------------
# The CMakeLists.txt for:                 uSimBeaconRange
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
   SBR_MOOSApp.cpp
   SBR_Model.cpp
   SBR_ExampleConfig.cpp
   BeaconBuoy.cpp
   main.cpp
)

ADD_EXECUTABLE(uSimBeaconRange ${SRC})

TARGET_LINK_LIBRARIES(uSimBeaconRange
  MOOS
  MOOSGen
  MOOSUtility
  mbutil
  geometry 
  contacts
  ${SYSTEM_LIBS}
)

# Install Targets
INSTALL(TARGETS uSimBeaconRange
  RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin
)
