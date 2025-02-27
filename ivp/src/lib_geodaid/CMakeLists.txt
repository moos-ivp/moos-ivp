#--------------------------------------------------------
# The CMakeLists.txt for:                     lib_geodaid
# Author(s):                                Mike Benjamin
#--------------------------------------------------------

SET(SRC
  ContactLedger.cpp
)

SET(HEADERS
  ContactLedger.h
)

# Build Library
ADD_LIBRARY(geodaid ${SRC})

