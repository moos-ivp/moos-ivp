#--------------------------------------------------------
# The CMakeLists.txt for:                            uXMS
# Author(s):                                Mike Benjamin
#--------------------------------------------------------

SET(SRC 
  XMS.cpp
  main_uXMS.cpp
)

ADD_EXECUTABLE(uXMS ${SRC})

TARGET_LINK_LIBRARIES(uXMS 
  MOOS
  MOOSGen
  genutil
  mbutil 
  m
  pthread)

