#--------------------------------------------------------
# The CMakeLists.txt for:               uFldCTDSensor
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
   CTDSensor_MOOSApp.cpp
   CTDSensor_Model.cpp
   CTDSensor_Info.cpp
   main.cpp
)

ADD_EXECUTABLE(uFldCTDSensor ${SRC})

TARGET_LINK_LIBRARIES(uFldCTDSensor
  ${MOOS_LIBRARIES}
 # MOOSUtility
  henrik_anneal
  contacts
  geometry 
  mbutil
  ${SYSTEM_LIBS}
)

# Install Targets
INSTALL(TARGETS uFldCTDSensor
  RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin
)
