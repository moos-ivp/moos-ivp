#--------------------------------------------------------
# The CMakeLists.txt for:                       lib_polar
# Author(s):                                Mike Benjamin
#--------------------------------------------------------

SET(SRC
  PolarPlot.cpp
  WindModel.cpp
  PopulatorPolarPlot.cpp
)

SET(HEADERS
  PolarPlot.h
  WindModel.h
  PopulatorPolarPlot.h
)

# Build Library
IVP_ADD_LIBRARY(polar ${SRC})
TARGET_LINK_LIBRARIES(polar
  PUBLIC geometry
  PRIVATE mbutil)
