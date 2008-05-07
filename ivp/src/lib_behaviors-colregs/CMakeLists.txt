#--------------------------------------------------------
# The CMakeLists.txt for:           lib_behaviors-colregs
# Author(s):                                Mike Benjamin
#--------------------------------------------------------

MACRO(ADD_BHV BHV_NAME)
  ADD_LIBRARY(${BHV_NAME} SHARED ${BHV_NAME}.cpp)
  TARGET_LINK_LIBRARIES(${BHV_NAME} 
    behaviors ivpbuild ivpcore bhvutil mbutil geometry m)
ENDMACRO(ADD_BHV)

ADD_BHV(BHV_HeadOn14)
ADD_BHV(BHV_StandOn17)
ADD_BHV(BHV_GiveWay16)

FILE(GLOB SRC
  AOF_R14.cpp
  AOF_R16.cpp
  AOF_R17.cpp
  BHV_GiveWay16.cpp
  BHV_HeadOn14.cpp
  BHV_StandOn17.cpp)

ADD_LIBRARY(behaviors-colregs ${SRC})
