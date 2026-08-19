#--------------------------------------------------------
# The CMakeLists.txt for:                    lib_contacts
# Author(s):                                Mike Benjamin
#--------------------------------------------------------

SET(SRC
  NodeRecord.cpp
  NodeRecordUtils.cpp
  NodeRider.cpp
  NodeRiderSet.cpp
)

SET(HEADERS
  NodeRecord.h
  NodeRecordUtils.h
  NodeRider.h
  NodeRiderSet.h
)

# Build Library
IVP_ADD_LIBRARY(contacts ${SRC})
TARGET_LINK_LIBRARIES(contacts
  PRIVATE geometry mbutil)
