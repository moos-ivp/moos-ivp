#--------------------------------------------------------
# The CMakeLists.txt for:                     lib_navplot
# Author(s):                                Mike Benjamin
#--------------------------------------------------------

SET(SRC
   IPF_Plot.cpp
   LogPlot.cpp
   GridPlot.cpp
   Populator_IPF_Plot.cpp
   Populator_LogPlots.cpp
   Populator_GridPlot.cpp
)

ADD_LIBRARY(navplot ${SRC})
TARGET_LINK_LIBRARIES(navplot ivpbuild)