#--------------------------------------------------------
# The CMakeLists.txt for:                      lib_ufield
# Author(s):                                Mike Benjamin
#--------------------------------------------------------

SET(SRC
  HostRecord.cpp
  HostRecordUtils.cpp
  UFieldUtils.cpp
)

SET(HEADERS
  HostRecord.h
  HostRecordUtils.h
  UFieldUtils.h
)

# Build Library
ADD_LIBRARY(ufield ${SRC})

