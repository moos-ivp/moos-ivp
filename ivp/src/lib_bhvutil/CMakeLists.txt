#--------------------------------------------------------
# The CMakeLists.txt for:                     lib_bhvutil
# Author(s):                                Mike Benjamin
#--------------------------------------------------------

SET(SRC
  AOF_Contact.cpp
  AOF_AttractorCPA.cpp
  AOF_AvoidCollision.cpp
  AOF_AvoidCollisionDepth.cpp
  AOF_AvoidObstacle.cpp
  AOF_AvoidObstacleX.cpp
  AOF_AvoidWalls.cpp
  AOF_CutRangeCPA.cpp
  AOF_Shadow.cpp
  AOF_Waypoint.cpp
  LoiterEngine.cpp
  ObShipModel.cpp
  ExFilterSet.cpp
  WaypointEngine.cpp
  WaypointEngineX.cpp
  RefineryObAvoid.cpp
  RefineryCPA.cpp
)

SET(HEADERS
  AOF_Contact.h
  AOF_ContactX.h
  AOF_AttractorCPA.h
  AOF_AvoidCollisionDepth.h
  AOF_AvoidCollision.h
  AOF_AvoidCollisionX.h
  AOF_AvoidObstacle.h
  AOF_AvoidObstacleX.h
  AOF_AvoidWalls.h
  AOF_CutRangeCPA.h
  AOF_Shadow.h
  AOF_Waypoint.h
  LoiterEngine.h
  ObShipModel.h
  ExFilterSet.h
  WaypointEngineX.h
  RefineryObAvoid.h
  RefineryCPA.h
)

# Build Library
ADD_LIBRARY(bhvutil ${SRC})

