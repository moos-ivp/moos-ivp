#--------------------------------------------------------
# The CMakeLists.txt for                     lib_ulogview
# Author(s):                                Mike Benjamin
#--------------------------------------------------------

SET(SRC
   LogTPlot.cpp
   TimePlot.cpp
   VPlugTPlot.cpp
   HelmTPlot.cpp
   IterTPlot.cpp
   VarTPlot.cpp
   VarTPlotSet.cpp
   IPF_TPlot.cpp
   ULV_TimeStamp.cpp
   GeneralLauncher.cpp
   PostingBuffer.cpp
   Populator_LogTPlots.cpp
   Populator_IPF_TPlot.cpp
   Populator_VPlugTPlots.cpp
)

SET(HEADERS
  HelmPlot.h
  IPF_Plot.h
  LogPlot.h
  Populator_HelmPlots.h
  Populator_IPF_Plot.h
  Populator_LogPlots.h
  Populator_VPlugPlots.h
  VPlugPlot.h
)

# Build Library
ADD_LIBRARY(ulogview ${SRC})
TARGET_LINK_LIBRARIES(ulogview ivpbuild)

