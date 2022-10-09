#--------------------------------------------------------
# The CMakeLists.txt for:                     lib_turngeo
# Author(s):                                Mike Benjamin
#--------------------------------------------------------

SET(SRC
  TurnGenerator.cpp
  TurnGen180.cpp
  TurnGenWilliamson.cpp
)

SET(HEADERS
  TurnGenerator.h
  TurnGen180.h
  TurnGenWilliamson.h
)

# Build Library
ADD_LIBRARY(turngeo ${SRC})

