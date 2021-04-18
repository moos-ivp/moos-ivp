#--------------------------------------------------------
# The CMakeLists.txt for:                  testPolyRayCPA
# Author(s):                                Mike Benjamin
#--------------------------------------------------------

FILE(GLOB SRC
  main.cpp)
  
ADD_EXECUTABLE(testPolyRayCPA ${SRC})
   				   
TARGET_LINK_LIBRARIES(testPolyRayCPA
  geometry
  mbutil
  m)

