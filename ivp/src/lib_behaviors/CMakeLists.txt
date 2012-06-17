#--------------------------------------------------------
# The CMakeLists.txt for:                   lib_behaviors
# Author(s):                                Mike Benjamin
#--------------------------------------------------------

SET(SRC
  IvPBehavior.cpp
  IvPContactBehavior.cpp
  LifeEventHistory.cpp
)

SET(HEADERS
  BehaviorReport.h
  BehaviorSet.h
  IvPBehavior.h
)

# Build Library
ADD_LIBRARY(behaviors ${SRC})

