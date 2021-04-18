#--------------------------------------------------------
# The CMakeLists.txt for:                testDistSegToArc
# Author(s):                                Mike Benjamin
#--------------------------------------------------------

FILE(GLOB SRC
  TestDistSegToArc.cpp
  main.cpp)
  
ADD_EXECUTABLE(testDistSegToArc ${SRC})
   				   
TARGET_LINK_LIBRARIES(testDistSegToArc
  geometry
  mbutil
  m)

