#--------------------------------------------------------
# The CMakeLists.txt for:                     lib_ivpcore
# Author(s):                                Mike Benjamin
#--------------------------------------------------------

SET(SRC
  BoxSet.cpp      
  IvPBox.cpp      
  IvPDomain.cpp   
  IvPFunction.cpp 
  IvPGrid.cpp     
  IvPProblem.cpp
  IvPProblem_v3.cpp
  PDMap.cpp
  Problem.cpp)

ADD_LIBRARY(ivpcore ${SRC})
