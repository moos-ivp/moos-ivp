#--------------------------------------------------------
# The CMakeLists.txt for:              testDistPointToArc
# Author(s):                                Mike Benjamin
#--------------------------------------------------------

FILE(GLOB SRC
  TestDistPointToArc.cpp
  main.cpp)
  
ADD_EXECUTABLE(testDistPointToArc ${SRC})
   				   
TARGET_LINK_LIBRARIES(testDistPointToArc
  geometry
  mbutil
  m)

