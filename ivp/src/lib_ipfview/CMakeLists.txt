#--------------------------------------------------------
# The CMakeLists.txt for:                     lib_ipfview
# Author(s):                                Mike Benjamin
#--------------------------------------------------------

FILE(GLOB SRC
  Common_IPFViewer.cpp
  QuadSet.cpp)


ADD_LIBRARY(ipfview ${SRC})

