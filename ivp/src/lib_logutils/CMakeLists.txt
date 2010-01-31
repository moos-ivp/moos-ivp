#--------------------------------------------------------
# The CMakeLists.txt for:                    lib_logutils
# Author(s):                                Mike Benjamin
#--------------------------------------------------------

SET(SRC
  ScanReport.cpp
  ALogScanner.cpp
  LogUtils.cpp
)

SET(HEADERS
  ALogEntry.h
  ALogScanner.h
  LogUtils.h
  ScanReport.h
)

# Build Library
ADD_LIBRARY(logutils ${SRC})

# Install Headers
INSTALL( FILES ${HEADERS} DESTINATION ${CMAKE_INSTALL_PREFIX}/include/lib_logutils)

# Install Libararies
INSTALL( TARGETS logutils
   RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
   LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
   ARCHIVE DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
)
