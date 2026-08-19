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
IVP_ADD_LIBRARY(geodaid ${SRC})
TARGET_LINK_LIBRARIES(geodaid
  PUBLIC contacts MOOSGeodesy::MOOSGeodesy
  PRIVATE geometry mbutil)
