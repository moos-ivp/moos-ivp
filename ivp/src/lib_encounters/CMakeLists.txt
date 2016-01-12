#--------------------------------------------------------
# The CMakeLists.txt for:                  lib_encounters
# Author(s):                                Mike Benjamin
#--------------------------------------------------------

SET(SRC
  CPAEvent.cpp
)

SET(HEADERS
  CPAEvent.h
)

# Build Library
ADD_LIBRARY(encounters ${SRC})

