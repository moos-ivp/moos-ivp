#--------------------------------------------------------
# The CMakeLists.txt for:                          alogrm
# Author(s):                                Mike Benjamin
#--------------------------------------------------------

# Set System Specific Libraries
if (${WIN32})
  SET(SYSTEM_LIBS
    wsock32)
else (${WIN32})
  SET(SYSTEM_LIBS
    m)
endif (${WIN32})

SET(SRC main.cpp FiltHandler.cpp)

ADD_EXECUTABLE(alogrm ${SRC})
   
TARGET_LINK_LIBRARIES(alogrm
  mbutil
  logutils
  ${SYSTEM_LIBS})


