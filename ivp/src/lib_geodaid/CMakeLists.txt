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
  PUBLIC contacts
  PRIVATE geometry mbutil)
IF(NOT IVP_BUILD_HELM_LIBS_ONLY)
  TARGET_LINK_LIBRARIES(geodaid PUBLIC MOOSGeodesy::MOOSGeodesy)
  TARGET_COMPILE_DEFINITIONS(geodaid PUBLIC IVP_CONTACT_LEDGER_MOOS_GEODESY)
ENDIF()
