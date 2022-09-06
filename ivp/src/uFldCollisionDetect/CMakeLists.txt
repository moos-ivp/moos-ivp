#--------------------------------------------------------
# The CMakeLists.txt for:             uFldCollisionDetect
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
  CollisionDetector.cpp
  CollisionDetector_Info.cpp
  CPAMonitor.cpp
  main.cpp
)
 
ADD_EXECUTABLE(uFldCollisionDetect ${SRC})

TARGET_LINK_LIBRARIES(uFldCollisionDetect
   ${MOOS_LIBRARIES}
   apputil
   contacts
   geometry
   mbutil
   ufield
   encounters
   logic
   ${SYSTEM_LIBS}
)
