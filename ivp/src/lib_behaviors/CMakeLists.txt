#--------------------------------------------------------
# The CMakeLists.txt for:                   lib_behaviors
# Author(s):                                Mike Benjamin
#--------------------------------------------------------

SET(SRC
  IvPBehavior.cpp
  IvPContactBehavior.cpp
#  IvPTaskBehavior.cpp
  LifeEventHistory.cpp
)

SET(HEADERS
  BehaviorReport.h
  BehaviorSet.h
  IvPBehavior.h
  IvPContactBehavior.h
#  IvPTaskBehavior.h
)

# Build Library
ADD_LIBRARY(behaviors ${SRC})

