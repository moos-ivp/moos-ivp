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
   NodeReporter_Info.cpp
   main.cpp
)

ADD_EXECUTABLE(pNodeReporter ${SRC})
   
TARGET_LINK_LIBRARIES(pNodeReporter
   ${MOOS_LIBRARIES}
   ${MOOSGeodesy_LIBRARIES}
   contacts
   apputil
   mbutil
   geometry
   ${SYSTEM_LIBS})

