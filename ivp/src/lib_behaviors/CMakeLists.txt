#--------------------------------------------------------
# The CMakeLists.txt for:                   lib_behaviors
# Author(s):                                Mike Benjamin
#--------------------------------------------------------

SET(SRC
  IvPBehavior.cpp
  IvPContactBehavior.cpp
#  IvPTaskBehavior.cpp
  LifeEventHistory.cpp
  LifeEvent.cpp
  ContactState.cpp
  ContactStateSet.cpp
)

SET(HEADERS
  BehaviorReport.h
  BehaviorSet.h
  IvPBehavior.h
  IvPContactBehavior.h
  LifeEventHistory.h
  LifeEvent.h
  ContactStateSet.h
#  IvPTaskBehavior.h
)

# Build Library
ADD_LIBRARY(behaviors ${SRC})

