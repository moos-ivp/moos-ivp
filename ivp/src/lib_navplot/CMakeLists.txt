#--------------------------------------------------------
# The CMakeLists.txt for:                     lib_navplot
# Author(s):                                Mike Benjamin
#--------------------------------------------------------

SET(SRC
   LogPlot.cpp
   VPlugPlot.cpp
   HelmPlot.cpp
   IPF_Plot.cpp
   ULV_TimeStamp.cpp
   Populator_LogPlots.cpp
   Populator_IPF_Plot.cpp
   Populator_HelmPlots.cpp
   Populator_VPlugPlots.cpp
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
ADD_LIBRARY(navplot ${SRC})
TARGET_LINK_LIBRARIES(navplot ivpbuild)

# Install Headers
INSTALL( FILES ${HEADERS} DESTINATION ${CMAKE_INSTALL_PREFIX}/include/lib_navplot)

# Install Libararies
INSTALL( TARGETS navplot
   RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
   LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
   ARCHIVE DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
)
