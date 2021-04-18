#--------------------------------------------------------
# The CMakeLists.txt for:                 testArcSegCross
# Author(s):                                Mike Benjamin
#--------------------------------------------------------

FILE(GLOB SRC
  TestArcSegCross.cpp
  main.cpp)
  
ADD_EXECUTABLE(testArcSegCross ${SRC})
   				   
TARGET_LINK_LIBRARIES(testArcSegCross
  geometry
  mbutil
  m)

