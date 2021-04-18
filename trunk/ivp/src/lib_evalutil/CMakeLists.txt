#--------------------------------------------------------
# The CMakeLists.txt for:                    lib_evalutil
# Author(s):                                Mike Benjamin
#--------------------------------------------------------

SET(SRC
  LCheckSet.cpp
  VCheck.cpp
  VCheckSet.cpp
  )

SET(HEADERS
  VCheckSet.h
  LCheckSet.h
  VCheck.h
)

# Build Library
ADD_LIBRARY(evalutil ${SRC})

