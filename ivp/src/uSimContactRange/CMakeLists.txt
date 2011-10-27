#--------------------------------------------------------
# The CMakeLists.txt for:                uSimContactRange
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
   SIMCOR_MOOSApp.cpp
   SIMCOR_Model.cpp
   SIMCOR_ExampleConfig.cpp
   main.cpp
)

ADD_EXECUTABLE(uSimContactRange ${SRC})

TARGET_LINK_LIBRARIES(uSimContactRange
  MOOS
  MOOSGen
  MOOSUtility
  mbutil
  geometry 
  contacts
  ${SYSTEM_LIBS}
)

# Install Targets
INSTALL(TARGETS uSimContactRange
  RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin
)
