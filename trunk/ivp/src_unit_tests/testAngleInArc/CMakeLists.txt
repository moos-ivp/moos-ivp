#--------------------------------------------------------
# The CMakeLists.txt for:                  testAngleinArc
# Author(s):                                Mike Benjamin
#--------------------------------------------------------

FILE(GLOB SRC
  main.cpp)
  
ADD_EXECUTABLE(testAngleInArc ${SRC})
   				   
TARGET_LINK_LIBRARIES(testAngleInArc
  geometry
  mbutil
  m)

