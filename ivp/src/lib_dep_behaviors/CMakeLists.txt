#--------------------------------------------------------
# The CMakeLists.txt for:               lib_dep_behaviors
# Author(s):                                Mike Benjamin
#--------------------------------------------------------

SET(SRC
  BHV_FixTurn.cpp
  BHV_RubberBand.cpp
  BHV_Attractor.cpp
  BHV_AvoidObstacle.cpp
  BHV_AvoidObstacleX.cpp
  BHV_AvoidObstacleV21.cpp
  ObShipModel.cpp
  AOF_AvoidObstacle.cpp
  AOF_AvoidObstacleX.cpp
  RefineryObAvoid.cpp
)

SET(HEADERS
  BHV_FixTurn.h
  BHV_RubberBand.h
  BHV_Attractor.h
  BHV_AvoidObstacle.h
  BHV_AvoidObstacleX.h
  ObShipModel.h
  AOF_AvoidObstacle.h
  AOF_AvoidObstacleX.h
  RefineryObAvoid.h
) 

ADD_LIBRARY(dep_behaviors ${SRC})

TARGET_LINK_LIBRARIES(dep_behaviors
   mbutil
   helmivp
   behaviors
   geometry
   )

