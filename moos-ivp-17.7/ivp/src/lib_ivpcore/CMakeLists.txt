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
  PDMap.cpp
)

SET(HEADERS
  BoxSet.h
  BoxSetNode.h
  Compactor.h
  CompactorNull.h
  IvPBox.h
  IvPDomain.h
  IvPFunction.h
  IvPGrid.h
  PDMap.h
)

# Build Library
ADD_LIBRARY(ivpcore ${SRC})

