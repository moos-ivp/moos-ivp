#--------------------------------------------------------
# The CMakeLists.txt for:                     lib_ipfview
# Author(s):                                Mike Benjamin
#--------------------------------------------------------

SET(SRC
  Common_IPFViewer.cpp
  QuadSet.cpp
  IPFViewUtils.cpp  
  IPF_Entry.cpp
  IPF_Bundle.cpp
  IPF_BundleSeries.cpp
)

SET(HEADERS
  IPFViewUtils.h  
  Common_IPFViewer.h
  Quad3D.h
  QuadSet.h
  IPF_Entry.h
  IPF_Bundle.h
  IPF_BundleSeries.h
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

