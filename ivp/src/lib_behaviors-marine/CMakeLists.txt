#--------------------------------------------------------
# The CMakeLists.txt for:            lib_behaviors-marine
# Author(s):                                Mike Benjamin
#--------------------------------------------------------

SET(SRC
  BHV_AbortToPoint.cpp
  BHV_AvoidCollision.cpp
  BHV_AvoidObstacleV24.cpp
  BHV_ConstantDepth.cpp
  BHV_ConstantHeading.cpp
  BHV_MaxDepth.cpp
  BHV_MaxSpeed.cpp
  BHV_MaintainHeading.cpp
  BHV_ConstantSpeed.cpp
  BHV_CutRange.cpp
  BHV_GoToDepth.cpp
  BHV_Hysteresis.cpp
  BHV_HeadingHysteresis.cpp
  BHV_Loiter.cpp
  BHV_LoiterV24.cpp
  BHV_MinAltitudeX.cpp
  BHV_MemoryTurnLimit.cpp
  BHV_OpRegion.cpp
  BHV_OpRegionRecover.cpp
  BHV_OpRegionV24.cpp
  BHV_PeriodicSpeed.cpp
  BHV_PeriodicSurface.cpp
  BHV_RStationKeep.cpp
  BHV_Shadow.cpp
  BHV_StationKeep.cpp
  BHV_TimeOut.cpp
  BHV_Timer.cpp
  BHV_Trail.cpp
  BHV_Convoy.cpp
  BHV_Waypoint.cpp
  BHV_LegRun.cpp
  BHV_LegRunX.cpp
  BHV_ZigZag.cpp
  BHV_FullStop.cpp
  BHV_HSLine.cpp
  BHV_HeadingChange.cpp
  BHV_HeadingBias.cpp
  BHV_BearingLine.cpp
  BHV_TestFailure.cpp
  BHV_PModelView.cpp
)

SET(HEADERS
  BHV_AbortToPoint.h
  BHV_AvoidCollision.h
  BHV_AvoidObstacleV24.h
  BHV_ConstantDepth.h
  BHV_ConstantHeading.h
  BHV_ConstantSpeed.h
  BHV_MaxDepth.h
  BHV_CutRange.h
  BHV_GoToDepth.h
  BHV_HeadingBias.h
  BHV_HeadingChange.h
  BHV_HeadingHysteresis.h
  BHV_HSLine.h
  BHV_Hysteresis.h
  BHV_Loiter.h
  BHV_LoiterV24.h
  BHV_MaintainHeading.h
  BHV_MinAltitudeX.h
  BHV_MemoryTurnLimit.h
  BHV_OpRegion.h
  BHV_OpRegionRecover.h
  BHV_OpRegionV24.h
  BHV_PeriodicSpeed.h
  BHV_RStationKeep.h
  BHV_Shadow.h
  BHV_StationKeep.h
  BHV_TimeOut.h
  BHV_Timer.h
  BHV_Trail.h
  BHV_Waypoint.h
  BHV_LegRun.h
  BHV_LegRunX.h
  BHV_ZigZag.h
  BHV_FullStop.h
#  BHV_FixTurn.h
  BHV_BearingLine.h
  BHV_TestFailure.h
  BHV_PModelView.h
) 

ADD_LIBRARY(behaviors-marine ${SRC})

TARGET_LINK_LIBRARIES(behaviors-marine
   mbutil
   helmivp
   behaviors
   geometry
   )

