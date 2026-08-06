#--------------------------------------------------------
# The CMakeLists.txt for:                   lib_obstacles
# Author(s):                                Mike Benjamin
#--------------------------------------------------------

SET(SRC
  ObstacleFieldGenerator.cpp
  Obstacle.cpp
)

SET(HEADERS
  ObstacleFieldGenerator.h
  Obstacle.h
)

# Build Library
IVP_ADD_LIBRARY(obstacles ${SRC})
TARGET_LINK_LIBRARIES(obstacles
  PUBLIC geometry
  PRIVATE mbutil)
