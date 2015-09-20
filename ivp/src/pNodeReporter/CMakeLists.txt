#--------------------------------------------------------
# The CMakeLists.txt for:                   pNodeReporter
# Author(s):                                Mike Benjamin
#--------------------------------------------------------

# Set System Specific Libraries
SET(SYSTEM_LIBS m pthread)

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

