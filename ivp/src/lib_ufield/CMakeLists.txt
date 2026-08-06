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
IVP_ADD_LIBRARY(ufield ${SRC})
TARGET_LINK_LIBRARIES(ufield PRIVATE mbutil)
IF(WIN32)
  TARGET_LINK_LIBRARIES(ufield PRIVATE ws2_32)
ENDIF()
