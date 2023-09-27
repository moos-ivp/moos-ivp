#--------------------------------------------------------
# The CMakeLists.txt for:                      app_projpt
# Author(s):                                Mike Benjamin
#--------------------------------------------------------

FILE(GLOB SRC
  main.cpp)

ADD_EXECUTABLE(projpt ${SRC})
   
TARGET_LINK_LIBRARIES(projpt
  geometry
  mbutil
  m)

