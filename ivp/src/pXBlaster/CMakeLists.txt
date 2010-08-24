#--------------------------------------------------------
# The CMakeLists.txt for:                       pXBlaster
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
   XBlaster.cpp  
   main.cpp
)  

ADD_EXECUTABLE(pXBlaster ${SRC}	)

TARGET_LINK_LIBRARIES(pXBlaster
   MOOS
   MOOSGen
   mbutil
   ${SYSTEM_LIBS})

# Install Targets
INSTALL(TARGETS pXBlaster
  RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin
)

