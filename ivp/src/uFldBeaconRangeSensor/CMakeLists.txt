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
   BRS_App.cpp
   BRS_Info.cpp
   BeaconBuoy.cpp
   main.cpp
)

ADD_EXECUTABLE(uFldBeaconRangeSensor ${SRC})

TARGET_LINK_LIBRARIES(uFldBeaconRangeSensor
  ${MOOS_LIBRARIES}
  ${MOOSGeodesy_LIBRARIES}
  mbutil
  apputil
  geodaid
  contacts
  geometry 
  ${SYSTEM_LIBS}
)

