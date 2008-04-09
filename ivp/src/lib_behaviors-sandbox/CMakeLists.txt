#--------------------------------------------------------
# The CMakeLists.txt for:           lib_behaviors-sandbox
# Author(s):                                Mike Benjamin
#--------------------------------------------------------

FILE(GLOB SRC
  AOF_AvoidCollisionTOL.cpp
  AOF_CutRangeFCPA.cpp
  AOF_GENERIC.cpp
  AOF_OpArea.cpp
  AOF_SearchGrid.cpp
  AOF_TOL.cpp
  AOF_Waypoint2D.cpp
  AOF_WaypointRateClosure.cpp
  BHV_AllStop.cpp
  BHV_AvoidCollisionTOL.cpp
  BHV_CoWaypoint.cpp
  BHV_CutRangeFCPA.cpp
  BHV_Generic.cpp
  BHV_LoiterMigrate.cpp
  BHV_ObeySpeedLimit.cpp
  BHV_OpArea.cpp
  BHV_SearchGrid.cpp
  BHV_SteadyHeading.cpp
  BHV_TOL.cpp
  BHV_Waypoint2D.cpp
  BHV_WaypointB.cpp
  BHV_WaypointRC.cpp
  BHV_WaypointSimple.cpp
  Migrator.cpp
  Timutif.cpp)

ADD_LIBRARY(behaviors-sandbox ${SRC})
