#--------------------------------------------------------
# The CMakeLists.txt for:                           pCBBA
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
   CBA_MOOSApp.cpp
   CBA_Model.cpp
   CBA_ExampleConfig.cpp
   Bundle.cpp
   Path.cpp
   main.cpp
)

ADD_EXECUTABLE(pCBBA ${SRC})

TARGET_LINK_LIBRARIES(pCBBA
  MOOS
  MOOSGen
  MOOSUtility
  mbutil
  ${SYSTEM_LIBS}
)

# Install Targets
INSTALL(TARGETS pCBBA
  RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin
)
