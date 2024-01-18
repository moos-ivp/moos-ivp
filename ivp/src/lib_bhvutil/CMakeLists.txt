#--------------------------------------------------------
# The CMakeLists.txt for:                     lib_bhvutil
# Author(s):                                Mike Benjamin
#--------------------------------------------------------

SET(SRC
  AOF_Contact.cpp
  AOF_AttractorCPA.cpp
  AOF_AvoidCollision.cpp
  AOF_AvoidCollisionDepth.cpp
  AOF_AvoidObstacleV24.cpp
  AOF_AvoidWalls.cpp
  AOF_CutRangeCPA.cpp
  AOF_Shadow.cpp
  AOF_Waypoint.cpp
  LoiterEngine.cpp
  ObShipModelV24.cpp
  ExFilterSet.cpp
  WaypointEngine.cpp
  WaypointEngineX.cpp
  RefineryObAvoidV24.cpp
  RefineryCPA.cpp
)

SET(HEADERS
  AOF_Contact.h
  AOF_ContactX.h
  AOF_AttractorCPA.h
  AOF_AvoidCollisionDepth.h
  AOF_AvoidCollision.h
  AOF_AvoidCollisionX.h
  AOF_AvoidObstacleV24.h
  AOF_AvoidWalls.h
  AOF_CutRangeCPA.h
  AOF_Shadow.h
  AOF_Waypoint.h
  LoiterEngine.h
  ObShipModelV24.h
  ExFilterSet.h
  WaypointEngineX.h
  RefineryObAvoidV24.h
  RefineryCPA.h
)

# Build Library
ADD_LIBRARY(bhvutil ${SRC})

