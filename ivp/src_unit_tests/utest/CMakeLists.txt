#--------------------------------------------------------
# The CMakeLists.txt for:                           utest
# Author(s):                                Mike Benjamin
#--------------------------------------------------------

FILE(GLOB SRC
  UnitTester.cpp
  main.cpp)

ADD_EXECUTABLE(utest ${SRC})
   
TARGET_LINK_LIBRARIES(utest
  mbutil
  m)

