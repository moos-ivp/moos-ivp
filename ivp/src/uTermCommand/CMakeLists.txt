#--------------------------------------------------------
# The CMakeLists.txt for:                    uTermCommand
# Author(s):                                Mike Benjamin
#--------------------------------------------------------

SET(SRC 
  TermCommand.cpp
  TermCommandMain.cpp)

ADD_EXECUTABLE(uTermCommand ${SRC})

TARGET_LINK_LIBRARIES(uTermCommand 
  MOOS
  MOOSGen
  mbutil
  genutil
  m
  pthread)
