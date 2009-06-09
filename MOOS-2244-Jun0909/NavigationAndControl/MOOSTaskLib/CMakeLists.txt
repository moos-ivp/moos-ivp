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
VariableHeadingTask.cpp
AGVHeadingSpeedTask.cpp
VehicleFrameWayPointTask.cpp
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
PilotTask.h
VariableHeadingTask.h
AGVHeadingSpeedTask.h
VehicleFrameWayPointTask.h

)



INCLUDE_DIRECTORIES(${MOOS_SOURCE_DIR}/Thirdparty/newmat)
INCLUDE_DIRECTORIES(${MOOS_SOURCE_DIR}/Thirdparty)
INCLUDE_DIRECTORIES(${MOOS_SOURCE_DIR}/Essentials)

ADD_LIBRARY(MOOSTask ${MOOSTASKLIB_SRCS})
