#--------------------------------------------------------
# The CMakeLists.txt for:                    testLeftTurn
# Author(s):                                Mike Benjamin
#--------------------------------------------------------

FILE(GLOB SRC
  main.cpp)
  
ADD_EXECUTABLE(testLeftTurn ${SRC})

TARGET_LINK_LIBRARIES(testLeftTurn
  geometry
  mbutil
  m)

