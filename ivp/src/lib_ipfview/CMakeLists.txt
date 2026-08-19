#--------------------------------------------------------
# The CMakeLists.txt for:                     lib_ipfview
# Author(s):                                Mike Benjamin
#--------------------------------------------------------

SET(SRC
  Common_IPFViewer.cpp
  Common_IPF_GUI.cpp
  Quad3D.cpp
  QuadSet.cpp
  QuadSet1D.cpp
  IPFViewUtils.cpp  
  IPF_Entry.cpp
  IPF_Bundle.cpp
  IPF_Utils.cpp
  IPF_BundleSeries.cpp
)

SET(HEADERS
  IPFViewUtils.h  
  Common_IPFViewer.h
  Common_IPF_GUI.h
  Quad3D.h
  QuadSet.h
  QuadSet1D.h
  IPF_Entry.h
  IPF_Bundle.h
  IPF_BundleSeries.h
)


# Build Library
IVP_ADD_LIBRARY(ipfview ${SRC})
TARGET_LINK_LIBRARIES(ipfview
  PUBLIC ivpbuild ivpcore ivpsolve mbutil
    "$<BUILD_INTERFACE:ivp_fltk>$<INSTALL_INTERFACE:IVP::FLTK>"
  PRIVATE geometry)

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-cast-function-type ")
