#--------------------------------------------------------
# The CMakeLists.txt for:                      lib_ufield
# Author(s):                                Mike Benjamin
#--------------------------------------------------------

SET(SRC
  HostRecord.cpp
  HostRecordUtils.cpp
  NodeMessage.cpp
  NodeMessageUtils.cpp
  UFieldUtils.cpp
)

SET(HEADERS
  HostRecord.h
  HostRecordUtils.h
  NodeMessage.h
  NodeMessageUtils.h
  UFieldUtils.h
)

# Build Library
ADD_LIBRARY(ufield ${SRC})

