#--------------------------------------------------------
# The CMakeLists.txt for:                       lib_logic
# Author(s):                                Mike Benjamin
#--------------------------------------------------------

SET(SRC
  ConditionalParam.cpp
  LogicCondition.cpp
  LogicUtils.cpp
  ParseNode.cpp
)

ADD_LIBRARY(logic ${SRC})
