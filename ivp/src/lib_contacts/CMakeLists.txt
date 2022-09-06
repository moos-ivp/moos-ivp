#--------------------------------------------------------
# The CMakeLists.txt for:                    lib_contacts
# Author(s):                                Mike Benjamin
#--------------------------------------------------------

SET(SRC
  NodeRecord.cpp
  NodeRecordUtils.cpp
  NodeRider.cpp
  NodeRiderSet.cpp
  ContactLedger.cpp
)

SET(HEADERS
  NodeRecord.h
  NodeRecordUtils.h
  NodeRider.h
  ContactLedger.h
  ContactLedger.h
)

# Build Library
ADD_LIBRARY(contacts ${SRC})

