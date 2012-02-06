#--------------------------------------------------------
# The CMakeLists.txt for:                uFldHazardSensor
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
   HazardSensor_MOOSApp.cpp
   HazardSensor_Model.cpp
   HazardSensor_Info.cpp
   main.cpp
)

ADD_EXECUTABLE(uFldHazardSensor ${SRC})

TARGET_LINK_LIBRARIES(uFldHazardSensor
  MOOS
  MOOSGen
  MOOSUtility
  mbutil
  geometry 
  contacts
  ${SYSTEM_LIBS}
)

# Install Targets
INSTALL(TARGETS uFldHazardSensor
  RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin
)
