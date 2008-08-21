#--------------------------------------------------------
# The CMakeLists.txt for:            lib_behaviors-marine
# Author(s):                                Mike Benjamin
#--------------------------------------------------------

SET(SRC
  BHV_AbortToPoint.cpp
  BHV_Attractor.cpp
  BHV_AvoidCollision.cpp
  BHV_AvoidCollisionDepth.cpp
  BHV_AvoidObstacles.cpp
  BHV_ConstantDepth.cpp
  BHV_ConstantHeading.cpp
  BHV_MaintainHeading.cpp
  BHV_ConstantSpeed.cpp
  BHV_CutRange.cpp
  BHV_GoToDepth.cpp
  BHV_Hysteresis.cpp
  BHV_Loiter.cpp
  BHV_MemoryTurnLimit.cpp
  BHV_OpRegion.cpp
  BHV_PeriodicSpeed.cpp
  BHV_PeriodicSurface.cpp
  BHV_RStationKeep.cpp
  BHV_RubberBand.cpp
  BHV_Shadow.cpp
  BHV_SimpleWaypoint.cpp
  BHV_StationKeep.cpp
  BHV_TimeOut.cpp
  BHV_Timer.cpp
  BHV_Trail.cpp
#   BHV_Trail_V1.cpp
  BHV_Waypoint.cpp
  BHV_HPeriodicSurface.cpp
)

ADD_LIBRARY(behaviors-marine ${SRC})
