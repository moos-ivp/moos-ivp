#--------------------------------------------------------
# The CMakeLists.txt for:                   testAngleDiff
# Author(s):                                Mike Benjamin
#--------------------------------------------------------

FILE(GLOB SRC
  main.cpp)
  
ADD_EXECUTABLE(testAngleDiff ${SRC})
   				   
TARGET_LINK_LIBRARIES(testAngleDiff
  geometry
  mbutil
  m)

