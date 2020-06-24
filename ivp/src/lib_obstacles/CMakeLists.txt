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
ADD_LIBRARY(obstacles ${SRC})

