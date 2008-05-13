#--------------------------------------------------------
# The CMakeLists.txt for:                      uHelmScope
# Author(s):                                Mike Benjamin
#--------------------------------------------------------

SET(SRC 
  HelmScope.cpp
  IterBlockHelm.cpp
  IterBlockBHV.cpp
  IterBlockPosts.cpp
  main.cpp
)

ADD_EXECUTABLE(uHelmScope ${SRC})

TARGET_LINK_LIBRARIES(uHelmScope 
  MOOS
  MOOSGen
  genutil
  mbutil 
  m
  pthread)

