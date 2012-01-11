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

# Install Headers
INSTALL( FILES ${HEADERS} DESTINATION ${CMAKE_INSTALL_PREFIX}/include/lib_ufield)

# Install Libararies
INSTALL( TARGETS ufield
   RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
   LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
   ARCHIVE DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
)
