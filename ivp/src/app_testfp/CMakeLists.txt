#--------------------------------------------------------
# The CMakeLists.txt for:                      app_testfp
# Author(s):                                Mike Benjamin
#--------------------------------------------------------

FILE(GLOB SRC
  main.cpp)

ADD_EXECUTABLE(testfp ${SRC})
   
TARGET_LINK_LIBRARIES(testfp
  geometry
  mbutil
  m)

