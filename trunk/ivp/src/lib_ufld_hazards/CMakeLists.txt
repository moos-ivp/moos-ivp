#--------------------------------------------------------
# The CMakeLists.txt for:                lib_ufld_hazards
# Author(s):                                Mike Benjamin
#--------------------------------------------------------

SET(SRC
  XYFormatUtilsHazard.cpp
  XYFormatUtilsHazardSet.cpp
  XYHazard.cpp
  XYHazardSet.cpp
  XYHazardRepEval.cpp
)

SET(HEADERS
  XYFormatUtilsHazard.h
  XYFormatUtilsHazardSet.h
  XYHazard.h
  XYHazardSet.h
  XYHazardRepEval.h
)

# Build Library
ADD_LIBRARY(ufld_hazards ${SRC})

