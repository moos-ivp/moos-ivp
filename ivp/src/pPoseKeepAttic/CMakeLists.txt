#--------------------------------------------------------
# The CMakeLists.txt for:                       pPoseKeep
# Author(s):                              
#--------------------------------------------------------

SET(SRC
  PoseKeep.cpp
  PoseKeep_Info.cpp
  main.cpp
)

ADD_EXECUTABLE(pPoseKeep ${SRC})

TARGET_LINK_LIBRARIES(pPoseKeep
   ${MOOS_LIBRARIES}
   apputil
   geometry
   mbutil
   m
   pthread)

