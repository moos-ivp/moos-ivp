#--------------------------------------------------------
# The CMakeLists.txt for:                            uXMS
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
  XMS.cpp
  XMS_Info.cpp
  main.cpp
)

ADD_EXECUTABLE(uXMS ${SRC})

TARGET_LINK_LIBRARIES(uXMS 
  MOOS
  MOOSGen
  genutil
  mbutil 
  ${SYSTEM_LIBS})

# Install Targets
INSTALL(TARGETS uXMS
  RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin
)

