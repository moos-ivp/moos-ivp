#--------------------------------------------------------
# The CMakeLists.txt for:               uFldGenericSensor
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
   GenericSensor_MOOSApp.cpp
   GenericSensor_Model.cpp
   GenericSensor_Info.cpp
   main.cpp
)

ADD_EXECUTABLE(uFldGenericSensor ${SRC})

TARGET_LINK_LIBRARIES(uFldGenericSensor
  MOOS
  MOOSGen
  MOOSUtility
  mbutil
  geometry 
  contacts
  ${SYSTEM_LIBS}
)

# Install Targets
INSTALL(TARGETS uFldGenericSensor
  RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin
)
