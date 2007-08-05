PROJECT(MOOSTaskLib)

SET(MOOSTASKLIB_SRCS
ConstantDepthTask.cpp
ConstantHeadingTask.cpp
DiveTask.cpp
EndMission.cpp
GoToDepth.cpp
GoToWayPoint.cpp
HoldingPatternTask.cpp
LimitAltitude.cpp
LimitBox.cpp
LimitDepth.cpp
MOOSBehaviour.cpp
MOOSTaskReader.cpp
OrbitTask.cpp
OverallTimeOut.cpp
PathAction.cpp
SeabedTrack.cpp
SteerThenDriveXYPatternTask.cpp
SurveyTask.cpp
ThirdPartyTask.cpp
TimerTask.cpp
TrackLineTask.cpp
XYPatternTask.cpp
ZPatternTask.cpp
PilotTask.cpp
ConstantDepthTask.h
ConstantHeadingTask.h
DiveTask.h
EndMission.h
GoToDepth.h
GoToWayPoint.h
HoldingPatternTask.h
LimitAltitude.h
LimitBox.h
LimitDepth.h
MOOSBehaviour.h
MOOSTaskReader.h
OrbitTask.h
OverallTimeOut.h
PathAction.h
SeabedTrack.h
SteerThenDriveXYPatternTask.h
SurveyTask.h
ThirdPartyTask.h
TimerTask.h
TrackLineTask.h
XYPatternTask.h
ZPatternTask.h
PilotTask.h)



INCLUDE_DIRECTORIES(${MOOS_SOURCE_DIR}/Thirdparty/newmat)
INCLUDE_DIRECTORIES(${MOOS_SOURCE_DIR}/Thirdparty)


ADD_LIBRARY(MOOSTask ${MOOSTASKLIB_SRCS})

#===============================================================================
# For installation and Debian packaging...
#===============================================================================

INSTALL(
   TARGETS MOOSTask
   DESTINATION lib/moos-${MOOS_VERSION}
   COMPONENT Moos_NavigationAndControl_Dev)

# Note: If there are any header files that are truly unneeded by users of this
# library, it should be OK to remove them from the PUBLIC_HEADERS variable,
# which in turn will prevent them from being installed. -CJC
SET(PUBLIC_HEADERS 
   ConstantDepthTask.h
   ConstantHeadingTask.h
   DiveTask.h
   EndMission.h
   GoToDepth.h
   GoToWayPoint.h
   HoldingPatternTask.h
   LimitAltitude.h
   LimitBox.h
   LimitDepth.h
   MOOSBehaviour.h
   MOOSTaskDefaults.h
   MOOSTaskLib.h
   MOOSTaskReader.h
   OrbitTask.h
   OverallTimeOut.h
   PathAction.h
   PilotTask.h
   SeabedTrack.h
   SteerThenDriveXYPatternTask.h
   SurveyTask.h
   ThirdPartyTask.h
   TimerTask.h
   TrackLineTask.h
   XYPatternTask.h
   ZPatternTask.h
)

INSTALL(
   FILES ${PUBLIC_HEADERS}
   DESTINATION include/moos-${MOOS_VERSION}/NavigationAndControl/MOOSTaskLib
   COMPONENT Moos_NavigationAndControl_Dev)

#===============================================================================
