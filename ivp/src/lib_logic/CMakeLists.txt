#--------------------------------------------------------
# The CMakeLists.txt for:                       lib_logic
# Author(s):                                Mike Benjamin
#--------------------------------------------------------

SET(SRC
  ConditionalParam.cpp
  LogicCondition.cpp
  LogicUtils.cpp
  LogicBuffer.cpp
  ParseNode.cpp
  InfoBuffer.cpp
  LedgerSnap.cpp
)

SET(HEADERS
  ConditionalParam.h
  LogicCondition.h
  LogicUtils.h
  LogicBuffer.h
  ParseNode.h
  InfoBuffer.h
  LedgerSnap.h
)

# Build Library
ADD_LIBRARY(logic ${SRC})

