#--------------------------------------------------------
# The CMakeLists.txt for:                         pXRelay
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
   PXR_MOOSApp.cpp  
   PXR_Info.cpp
   main.cpp
)  

ADD_EXECUTABLE(pXRelay ${SRC}	)

TARGET_LINK_LIBRARIES(pXRelay
   MOOS
   MOOSGen
   mbutil
   ${SYSTEM_LIBS})

# Install Targets
INSTALL(TARGETS pXRelay
  RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin
)

