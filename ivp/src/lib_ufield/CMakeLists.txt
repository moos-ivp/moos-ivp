#--------------------------------------------------------
# The CMakeLists.txt for:                      lib_ufield
# Author(s):                                Mike Benjamin
#--------------------------------------------------------

SET(SRC
  HostRecord.cpp
  HostRecordUtils.cpp
  UFieldUtils.cpp
  CommsCatalog.cpp
  CommsEntry.cpp
  )

SET(HEADERS
  HostRecord.h
  HostRecordUtils.h
  UFieldUtils.h
  CommsCatalog.h
  CommsEntry.h
)

# Build Library
ADD_LIBRARY(ufield ${SRC})

