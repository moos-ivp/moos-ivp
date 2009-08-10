#--------------------------------------------------------
# The CMakeLists.txt for:                     lib_navplot
# Author(s):                                Mike Benjamin
#--------------------------------------------------------

SET(SRC
   # IPF_Plot.cpp
   # GridPlot.cpp
   # Populator_IPF_Plot.cpp
   # Populator_GridPlot.cpp
   LogPlot.cpp
   GeoPlot.cpp
   HelmPlot.cpp
   Populator_LogPlots.cpp
   Populator_HelmPlots.cpp
   Populator_GeoPlots.cpp
)

ADD_LIBRARY(navplot ${SRC})
TARGET_LINK_LIBRARIES(navplot ivpbuild)
