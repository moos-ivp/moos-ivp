#--------------------------------------------------------
# The CMakeLists.txt for:                testIncIntString
# Author(s):                                Mike Benjamin
#--------------------------------------------------------

FILE(GLOB SRC
  main.cpp)
  
ADD_EXECUTABLE(testIncIntString ${SRC})
   				   
TARGET_LINK_LIBRARIES(testIncIntString
  geometry
  mbutil
  m)

