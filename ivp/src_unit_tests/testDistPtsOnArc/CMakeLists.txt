#--------------------------------------------------------
# The CMakeLists.txt for:                testDistPtsOnArc
# Author(s):                                Mike Benjamin
#--------------------------------------------------------

FILE(GLOB SRC
  main.cpp)
  
ADD_EXECUTABLE(testDistPtsOnArc ${SRC})
   				   
TARGET_LINK_LIBRARIES(testDistPtsOnArc
  geometry
  mbutil
  m)

