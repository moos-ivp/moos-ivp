#--------------------------------------------------------
# The CMakeLists.txt for:           lib_behaviors-colregs
# Author(s):                                Mike Benjamin
#--------------------------------------------------------

FILE(GLOB SRC
  AOF_R14.cpp
  AOF_R16.cpp
  AOF_R17.cpp
  BHV_GiveWay16.cpp
  BHV_HeadOn14.cpp
  BHV_StandOn17.cpp)

ADD_LIBRARY(behaviors-colregs ${SRC})
