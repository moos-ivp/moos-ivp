#--------------------------------------------------------
# The CMakeLists.txt for:                     lib_navplot
# Author(s):                                Mike Benjamin
#--------------------------------------------------------

SET(SRC
   LogPlot.cpp
   VPlugPlot.cpp
   HelmPlot.cpp
   IPF_Plot.cpp
   Populator_LogPlots.cpp
   Populator_IPF_Plot.cpp
   Populator_HelmPlots.cpp
   Populator_VPlugPlots.cpp
)

ADD_LIBRARY(navplot ${SRC})
TARGET_LINK_LIBRARIES(navplot ivpbuild)
