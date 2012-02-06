#--------------------------------------------------------
# The CMakeLists.txt for:           uFldBeaconRangeSensor
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
   BRS_MOOSApp.cpp
   BRS_Model.cpp
   BRS_Info.cpp
   BeaconBuoy.cpp
   main.cpp
)

ADD_EXECUTABLE(uFldBeaconRangeSensor ${SRC})

TARGET_LINK_LIBRARIES(uFldBeaconRangeSensor
  MOOS
  MOOSGen
  MOOSUtility
  mbutil
  geometry 
  contacts
  ${SYSTEM_LIBS}
)

# Install Targets
INSTALL(TARGETS uFldBeaconRangeSensor
  RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin
)
