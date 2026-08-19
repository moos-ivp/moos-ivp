#--------------------------------------------------------
# The CMakeLists.txt for:                    lib_evalutil
# Author(s):                                Mike Benjamin
#--------------------------------------------------------

SET(SRC
  LogicAspect.cpp
  LogicTestSequence.cpp
  VCheck.cpp
  VCheckSet.cpp
  )

SET(HEADERS
  LogicAspect.h
  LogicTestSequence.h
  VCheckSet.h
  VCheck.h
)

# Build Library
IVP_ADD_LIBRARY(evalutil ${SRC})
TARGET_LINK_LIBRARIES(evalutil
  PUBLIC logic mbutil
  PRIVATE apputil)
