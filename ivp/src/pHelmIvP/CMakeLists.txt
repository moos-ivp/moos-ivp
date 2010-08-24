#--------------------------------------------------------
# The CMakeLists.txt for:                        pHelmIvP
# Author(s):                                Mike Benjamin
#--------------------------------------------------------

# Set System Specific Libraries
if (${WIN32})
  SET(SYSTEM_LIBS
    wsock32)
else (${WIN32})
  SET(SYSTEM_LIBS
    dl
    m
    pthread)
endif (${WIN32})

SET(SRC
  HelmIvP.cpp
  pHelmIvPMain.cpp
)

ADD_EXECUTABLE(pHelmIvP ${SRC})
   
TARGET_LINK_LIBRARIES(pHelmIvP 
  MOOS
  MOOSGen
  helmivp
  behaviors-marine
  behaviors
  bhvutil	
  ivpbuild 
  ivpcore 
  geometry
  mbutil 
  logic 
  genutil
  ${SYSTEM_LIBS})

# Install Targets
INSTALL(TARGETS pHelmIvP
  RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin
)

