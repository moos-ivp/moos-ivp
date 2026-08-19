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
IVP_ADD_LIBRARY(encounters ${SRC})
TARGET_LINK_LIBRARIES(encounters
  PRIVATE mbutil)
