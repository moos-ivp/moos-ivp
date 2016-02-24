#--------------------------------------------------------
# The CMakeLists.txt for:                    app_cpabench
# Author(s):                                Mike Benjamin
#--------------------------------------------------------

FILE(GLOB SRC
  main.cpp)

ADD_EXECUTABLE(cpabench ${SRC})
   
TARGET_LINK_LIBRARIES(cpabench
  geometry
  mbutil
  m)

