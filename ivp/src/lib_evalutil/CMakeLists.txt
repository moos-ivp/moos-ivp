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
ADD_LIBRARY(evalutil ${SRC})

