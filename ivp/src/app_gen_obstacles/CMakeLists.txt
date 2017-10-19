#--------------------------------------------------------
# The CMakeLists.txt for:                   gen_obstacles
# Author(s):                                Mike Benjamin
#--------------------------------------------------------

SET(SRC
  ObstacleFieldGenerator.cpp
  ObstacleFieldGenerator_Info.cpp
  main.cpp
)

ADD_EXECUTABLE(gen_obstacles ${SRC})

TARGET_LINK_LIBRARIES(gen_obstacles
  geometry
  mbutil
  ${SYSTEM_LIBS}
)
