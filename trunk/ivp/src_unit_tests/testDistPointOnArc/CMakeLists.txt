#--------------------------------------------------------
# The CMakeLists.txt for:              testDistPointOnArc
# Author(s):                                Mike Benjamin
#--------------------------------------------------------

FILE(GLOB SRC
  main.cpp)
  
ADD_EXECUTABLE(testDistPointOnArc ${SRC})
   				   
TARGET_LINK_LIBRARIES(testDistPointOnArc
  geometry
  mbutil
  m)

