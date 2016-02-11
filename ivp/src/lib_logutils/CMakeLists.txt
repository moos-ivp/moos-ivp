#--------------------------------------------------------
# The CMakeLists.txt for:                    lib_logutils
# Author(s):                                Mike Benjamin
#--------------------------------------------------------

SET(SRC
  ScanReport.cpp
  ALogScanner.cpp
  ALogSorter.cpp
  LogUtils.cpp
  ALogEntry.cpp
  SplitHandler.cpp  
  ALogDataBroker.cpp
  LogPlot.cpp
  VarPlot.cpp
  HelmPlot.cpp
  EncounterPlot.cpp
  VPlugPlot.cpp
  IPF_Plot.cpp
  VarPlotEntry.cpp
  Populator_VPlugPlots.cpp
  Populator_HelmPlots.cpp
  Populator_IPF_Plot.cpp
  Populator_EncounterPlot.cpp
  ModelHelmScope.cpp
  ModelVarScope.cpp
)

SET(HEADERS
   ALogEntry.h
   ALogScanner.h
   ALogSorter.h
   LogUtils.h
   ScanReport.h
   SplitHandler.h
)

# Build Library
ADD_LIBRARY(logutils ${SRC})

