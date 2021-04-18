#--------------------------------------------------------
# The CMakeLists.txt for:            testDistPointToArcPt
# Author(s):                                Mike Benjamin
#--------------------------------------------------------

FILE(GLOB SRC
  main.cpp)
  
ADD_EXECUTABLE(testDistPointToArcPt ${SRC})
   				   
TARGET_LINK_LIBRARIES(testDistPointToArcPt
  geometry
  mbutil
  m)

