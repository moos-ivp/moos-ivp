#--------------------------------------------------------
# The CMakeLists.txt for:                    lib_ivpsolve
# Author(s):                                Mike Benjamin
#--------------------------------------------------------

SET(SRC
  IvPProblem.cpp
  IvPProblem_v3.cpp
  IvPProblem_v2.cpp
  PopulatorIPP.cpp
  Problem.cpp
)

SET(HEADERS
  IvPProblem.h
  IvPProblem_v3.h
  IvPProblem_v2.h
  PopulatorIPP.h
  Problem.h
)

# Build Library
ADD_LIBRARY(ivpsolve ${SRC})

