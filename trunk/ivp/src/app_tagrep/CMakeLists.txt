#--------------------------------------------------------
# The CMakeLists.txt for:                          tagrep
# Author(s):                                Mike Benjamin
#--------------------------------------------------------

# Set System Specific Libraries
SET(SYSTEM_LIBS m)

SET(SRC main.cpp TagHandler.cpp)

ADD_EXECUTABLE(tagrep ${SRC})
   
TARGET_LINK_LIBRARIES(tagrep
  mbutil
  ${SYSTEM_LIBS})

