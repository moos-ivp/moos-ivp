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

SET(HEADERS
  AOF_AttractorCPA.h
  AOF_AvoidCollisionDepth.h
  AOF_AvoidCollision.h
  AOF_AvoidObstacles.h
#  AOF_CutRangeCPA.h
  AOF_Shadow.h
  AOF_Waypoint.h
  LoiterEngine.h
  WaypointEngine.h
)

# Build Library
ADD_LIBRARY(bhvutil ${SRC})

# Install Headers
INSTALL( FILES ${HEADERS} DESTINATION ${CMAKE_INSTALL_PREFIX}/include/lib_bhvutil)

# Install Libararies
INSTALL( TARGETS bhvutil
   RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
   LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
   ARCHIVE DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
)
