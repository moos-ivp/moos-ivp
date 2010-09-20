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
)

SET(HEADERS
  ConditionalParam.h
  InfoBuffer.h
  LogicCondition.h
  LogicUtils.h
  ParseNode.h
)

# Build Library
ADD_LIBRARY(logic ${SRC})

# Install Headers
INSTALL( FILES ${HEADERS} DESTINATION ${CMAKE_INSTALL_PREFIX}/include/lib_logic)

# Install Libararies
INSTALL( TARGETS logic
   RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
   LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
   ARCHIVE DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
)
