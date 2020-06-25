#--------------------------------------------------------
# The CMakeLists.txt for:                  testConvexHull
# Author(s):                                Mike Benjamin
#--------------------------------------------------------

FILE(GLOB SRC
  main.cpp)
  
ADD_EXECUTABLE(testConvexHull ${SRC})
   				   
TARGET_LINK_LIBRARIES(testConvexHull
  geometry
  mbutil
  m)

