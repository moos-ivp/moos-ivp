#--------------------------------------------------------
# The CMakeLists.txt for:                   uProcessWatch
# Author(s):                                Mike Benjamin
#--------------------------------------------------------

SET(SRC 
  ProcessWatch.cpp 
  main.cpp)

ADD_EXECUTABLE(uProcessWatch ${SRC})
 
TARGET_LINK_LIBRARIES(uProcessWatch 
  MOOS
  MOOSGen
  mbutil
  pthread)
