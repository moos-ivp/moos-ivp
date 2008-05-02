#--------------------------------------------------------
# The CMakeLists.txt for:                   lib_behaviors
# Author(s):                                Mike Benjamin
#--------------------------------------------------------

FILE(GLOB SRC
  BehaviorSet.cpp
  InfoBuffer.cpp
  IvPBehavior.cpp
  VarDataPair.cpp)

ADD_LIBRARY(behaviors ${SRC})
