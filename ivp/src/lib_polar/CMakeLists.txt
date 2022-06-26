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
ADD_LIBRARY(polar ${SRC})

