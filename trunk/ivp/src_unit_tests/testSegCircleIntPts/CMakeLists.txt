#--------------------------------------------------------
# The CMakeLists.txt for:             testSegCircleIntPts
# Author(s):                                Mike Benjamin
#--------------------------------------------------------

FILE(GLOB SRC
  TestSegCircleIntPts.cpp
  main.cpp)
  
ADD_EXECUTABLE(testSegCircleIntPts ${SRC})
   				   
TARGET_LINK_LIBRARIES(testSegCircleIntPts
  geometry
  mbutil
  m)

