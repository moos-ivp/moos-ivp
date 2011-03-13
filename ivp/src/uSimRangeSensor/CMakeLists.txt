#--------------------------------------------------------
# The CMakeLists.txt for:                 uSimRangeSensor
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
   SRS_MOOSApp.cpp
   SRS_Model.cpp
   BeaconBuoy.cpp
   NodeRecord.cpp
   main.cpp
)

ADD_EXECUTABLE(uSimRangeSensor ${SRC})

TARGET_LINK_LIBRARIES(uSimRangeSensor
  MOOS
  MOOSGen
  MOOSUtility
  mbutil
  geometry 
  ${SYSTEM_LIBS}
)

# Install Targets
INSTALL(TARGETS uSimRangeSensor
  RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin
)
