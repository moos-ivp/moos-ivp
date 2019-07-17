#--------------------------------------------------------
# The CMakeLists.txt for:                   testSegRayCPA
# Author(s):                                Mike Benjamin
#--------------------------------------------------------

FILE(GLOB SRC
  main.cpp)
  
ADD_EXECUTABLE(testSegRayCPA ${SRC})
   				   
TARGET_LINK_LIBRARIES(testSegRayCPA
  geometry
  mbutil
  m)

