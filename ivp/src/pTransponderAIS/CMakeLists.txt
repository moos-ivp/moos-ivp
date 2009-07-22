#--------------------------------------------------------
# The CMakeLists.txt for:                 pTransponderAIS
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
   TransponderAIS.cpp
   mainTransponderAIS.cpp
)

ADD_EXECUTABLE(pTransponderAIS ${SRC})
   
TARGET_LINK_LIBRARIES(pTransponderAIS 
   MOOS
   MOOSGen
   MOOSUtility
   mbutil
   ${SYSTEM_LIBS})
