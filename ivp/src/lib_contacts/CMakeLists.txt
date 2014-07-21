#--------------------------------------------------------
# The CMakeLists.txt for:                    lib_contacts
# Author(s):                                Mike Benjamin
#--------------------------------------------------------

SET(SRC
  NodeRecord.cpp
  NodeRecordUtils.cpp
)

SET(HEADERS
  ContactRecord.h
)

# Build Library
ADD_LIBRARY(contacts ${SRC})

