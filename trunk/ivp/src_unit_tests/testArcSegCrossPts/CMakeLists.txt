#--------------------------------------------------------
# The CMakeLists.txt for:              testArcSegCrossPts
# Author(s):                                Mike Benjamin
#--------------------------------------------------------

FILE(GLOB SRC
  TestArcSegCrossPts.cpp
  main.cpp)
  
ADD_EXECUTABLE(testArcSegCrossPts ${SRC})
   				   
TARGET_LINK_LIBRARIES(testArcSegCrossPts
  geometry
  mbutil
  m)

