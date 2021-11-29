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
  AppLogPlot.cpp
  AppLogEntry.cpp
  SplitHandler.cpp  
  ALogDataBroker.cpp
  LogPlot.cpp
  VarPlot.cpp
  HelmPlot.cpp
  TaskDiary.cpp
  EncounterPlot.cpp
  VPlugPlot.cpp
  IPF_Plot.cpp
  VarPlotEntry.cpp
  Populator_VPlugPlots.cpp
  Populator_HelmPlots.cpp
  Populator_IPF_Plot.cpp
  Populator_EncounterPlot.cpp
  Populator_AppLogPlot.cpp
  Populator_TaskDiary.cpp
  ModelHelmScope.cpp
  ModelVarScope.cpp
  ModelAppLogScope.cpp
  ModelTaskDiary.cpp
)

SET(HEADERS
   ALogEntry.h
   AppLogPlot.h
   AppLogEntry.h
   ALogScanner.h
   ALogSorter.h
   LogUtils.h
   ScanReport.h
   SplitHandler.h
   Populator_VPlugPlots.h
   Populator_HelmPlots.h
   Populator_IPF_Plot.h
   Populator_EncounterPlot.h
   Populator_AppLogPlot.h
   Populator_TaskDiary.h
   ModelHelmScope.h
   ModelVarScope.h
   ModelAppLogScope.h
   ModelTaskDiary.h
)

# Build Library
ADD_LIBRARY(logutils ${SRC})

