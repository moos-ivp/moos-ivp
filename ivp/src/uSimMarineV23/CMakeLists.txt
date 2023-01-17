#--------------------------------------------------------
# The CMakeLists.txt for:                   uSimMarineV23
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
   TurnSpeedMap.cpp
   WormHole.cpp
   WormHoleSet.cpp
   main.cpp
)

ADD_EXECUTABLE(uSimMarineV23 ${SRC})

TARGET_LINK_LIBRARIES(uSimMarineV23
  ${MOOS_LIBRARIES}
  ${MOOSGeodesy_LIBRARIES}
  polar
  marine_pid
  contacts
  geometry 
  apputil
  mbutil
  ${SYSTEM_LIBS})


