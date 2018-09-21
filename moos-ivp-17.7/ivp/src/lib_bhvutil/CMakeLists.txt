#--------------------------------------------------------
# The CMakeLists.txt for:                     lib_bhvutil
# Author(s):                                Mike Benjamin
#--------------------------------------------------------

SET(SRC
  AOF_Contact.cpp
  AOF_AttractorCPA.cpp
  AOF_AvoidCollision.cpp
  AOF_AvoidCollisionDepth.cpp
  AOF_AvoidObstacles.cpp
  AOF_AvoidObstacle.cpp
  AOF_CutRangeCPA.cpp
  AOF_Shadow.cpp
  AOF_Waypoint.cpp
  LoiterEngine.cpp
  WaypointEngine.cpp)

SET(HEADERS
  AOF_Contact.h
  AOF_AttractorCPA.h
  AOF_AvoidCollisionDepth.h
  AOF_AvoidCollision.h
  AOF_AvoidObstacles.h
  AOF_AvoidObstacle.h
  AOF_CutRangeCPA.h
  AOF_Shadow.h
  AOF_Waypoint.h
  LoiterEngine.h
  WaypointEngine.h
)

# Build Library
ADD_LIBRARY(bhvutil ${SRC})

