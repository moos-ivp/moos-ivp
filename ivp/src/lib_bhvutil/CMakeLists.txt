#--------------------------------------------------------
# The CMakeLists.txt for:                     lib_bhvutil
# Author(s):                                Mike Benjamin
#--------------------------------------------------------

SET(SRC
  AOF_AttractorCPA.cpp
  AOF_AvoidCollision.cpp
  AOF_AvoidCollisionDepth.cpp
  AOF_AvoidObstacles.cpp
  AOF_CutRangeCPA.cpp
#   AOF_CutRangeCPA_V1.cpp
  AOF_Shadow.cpp
  AOF_Waypoint.cpp
  LoiterEngine.cpp
  WaypointEngine.cpp)

ADD_LIBRARY(bhvutil ${SRC})
