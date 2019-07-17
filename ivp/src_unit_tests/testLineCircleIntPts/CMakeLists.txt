#--------------------------------------------------------
# The CMakeLists.txt for:            testLineCircleIntPts
# Author(s):                                Mike Benjamin
#--------------------------------------------------------

FILE(GLOB SRC
  TestLineCircleIntPts.cpp
  main.cpp)

ADD_EXECUTABLE(testLineCircleIntPts ${SRC})
   
TARGET_LINK_LIBRARIES(testLineCircleIntPts
  geometry
  mbutil
  m)

