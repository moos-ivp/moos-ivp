#--------------------------------------------------------
# The CMakeLists.txt for:                       uRepeater
# Author(s):                                Mike Benjamin
#--------------------------------------------------------

SET(SRC
  Repeater.cpp
  RepeaterMain.cpp
)

ADD_EXECUTABLE(uRepeater ${SRC})
 
TARGET_LINK_LIBRARIES(uRepeater
  ${MOOS_LIBS}
  mbutil
  pthread)
