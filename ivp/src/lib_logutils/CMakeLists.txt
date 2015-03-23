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

