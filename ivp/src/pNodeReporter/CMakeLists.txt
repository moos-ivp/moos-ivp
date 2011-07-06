#--------------------------------------------------------
# The CMakeLists.txt for:                   pNodeReporter
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
   NodeReporter.cpp
   PNR_ExampleConfig.cpp
   main.cpp
)

ADD_EXECUTABLE(pNodeReporter ${SRC})
   
TARGET_LINK_LIBRARIES(pNodeReporter
   MOOS
   MOOSGen
   MOOSUtility
   contacts
   mbutil
   geometry
   ${SYSTEM_LIBS})

# Install Targets
INSTALL(TARGETS pNodeReporter
  RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin
)
