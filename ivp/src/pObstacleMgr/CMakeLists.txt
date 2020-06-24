#--------------------------------------------------------
# The CMakeLists.txt for:                    pObstacleMgr
# Author(s):                                   M.Benjamin   
#--------------------------------------------------------

SET(SRC
  ObstacleManager.cpp
  ObstacleManager_Info.cpp
  main.cpp
)

ADD_EXECUTABLE(pObstacleMgr ${SRC})

TARGET_LINK_LIBRARIES(pObstacleMgr
   ${MOOS_LIBRARIES}
   apputil
   obstacles
   geometry
   mbutil
   m
   pthread)

