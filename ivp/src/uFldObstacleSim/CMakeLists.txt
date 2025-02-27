#--------------------------------------------------------
# The CMakeLists.txt for:                 uFldObstacleSim
# Author(s):                              
#--------------------------------------------------------

SET(SRC
  ObstacleSim.cpp
  ObstacleSim_Info.cpp
  main.cpp
)

ADD_EXECUTABLE(uFldObstacleSim ${SRC})

TARGET_LINK_LIBRARIES(uFldObstacleSim
   ${MOOS_LIBRARIES}
   ${MOOSGeodesy_LIBRARIES}
   apputil
   obstacles
   geodaid
   contacts
   geometry
   mbutil
   m
   pthread)

