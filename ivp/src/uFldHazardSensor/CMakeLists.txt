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
   HazardSensor_Info.cpp
   HeadingHistory.cpp
   main.cpp
)

ADD_EXECUTABLE(uFldHazardSensor ${SRC})

TARGET_LINK_LIBRARIES(uFldHazardSensor
  ${MOOS_LIBRARIES}
  ufld_hazards
  mbutil
  geometry 
  contacts
  apputil
  ${SYSTEM_LIBS}
)


