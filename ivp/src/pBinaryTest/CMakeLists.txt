#--------------------------------------------------------
# The CMakeLists.txt for:                     pBinaryTest
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
   main.cpp
)  

ADD_EXECUTABLE(pBinaryTest ${SRC}	)

TARGET_LINK_LIBRARIES(pBinaryTest
   MOOS
   MOOSGen
   ${SYSTEM_LIBS})

# Install Targets
INSTALL(TARGETS pBinaryTest
  RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin
)

