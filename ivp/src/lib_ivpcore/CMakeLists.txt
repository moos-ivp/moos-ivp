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

SET(HEADERS
  BoxSet.h
  BoxSetNode.h
  Compactor.h
  CompactorNull.h
  IvPBox.h
  IvPDomain.h
  IvPFunction.h
  IvPGrid.h
  IvPProblem.h
  IvPProblem_v3.h
  PDMap.h
  Problem.h
)

# Build Library
ADD_LIBRARY(ivpcore ${SRC})

# Install Headers
INSTALL( FILES ${HEADERS} DESTINATION ${CMAKE_INSTALL_PREFIX}/include/lib_ivpcore)

# Install Libararies
INSTALL( TARGETS ivpcore
   RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
   LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
   ARCHIVE DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
)
