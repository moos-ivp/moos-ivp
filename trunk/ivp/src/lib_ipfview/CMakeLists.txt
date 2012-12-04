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


