#--------------------------------------------------------
# The CMakeLists.txt for:                     lib_ipfview
# Author(s):                                Mike Benjamin
#--------------------------------------------------------

SET(SRC
  Common_IPFViewer.cpp
  QuadSet.cpp
)

SET(HEADERS
  Common_IPFViewer.h
  Quad3D.h
  QuadSet.h
)

# Build Library
ADD_LIBRARY(ipfview ${SRC})

# Install Headers
INSTALL( FILES ${HEADERS} DESTINATION ${CMAKE_INSTALL_PREFIX}/include/lib_ipfview)

# Install Libararies
INSTALL( TARGETS ipfview
   RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
   LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
   ARCHIVE DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
)

