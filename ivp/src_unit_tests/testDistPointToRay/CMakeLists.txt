#--------------------------------------------------------
# The CMakeLists.txt for:              testDistPointToRay
# Author(s):                                Mike Benjamin
#--------------------------------------------------------

FILE(GLOB SRC
  main.cpp)
  
ADD_EXECUTABLE(testDistPointToRay ${SRC})
   				   
TARGET_LINK_LIBRARIES(testDistPointToRay
  geometry
  mbutil
  m)

