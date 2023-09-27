#--------------------------------------------------------
# The CMakeLists.txt for:                       app_nzero
# Author(s):                                Mike Benjamin
#--------------------------------------------------------

FILE(GLOB SRC
  main.cpp)

ADD_EXECUTABLE(nzero ${SRC})
   
TARGET_LINK_LIBRARIES(nzero
  geometry
  mbutil
  m)

