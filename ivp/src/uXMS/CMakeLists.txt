#--------------------------------------------------------
# The CMakeLists.txt for:                            uXMS
# Author(s):                                Mike Benjamin
#--------------------------------------------------------

FILE(GLOB SRC 
  XMS.cpp
  uXMSMain.cpp)

ADD_EXECUTABLE(uXMS ${SRC})

TARGET_LINK_LIBRARIES(uXMS 
  MOOS
  MOOSGen
  genutil
  mbutil 
  m
  pthread)

