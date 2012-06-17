#--------------------------------------------------------
# The CMakeLists.txt for:                      lib_ufield
# Author(s):                                Mike Benjamin
#--------------------------------------------------------

SET(SRC
  HostRecord.cpp
  HostRecordUtils.cpp
  NodeMessage.cpp
  NodeMessageUtils.cpp
)

SET(HEADERS
  HostRecord.h
  HostRecordUtils.h
  NodeMessageUtils.h
)

# Build Library
ADD_LIBRARY(ufield ${SRC})

