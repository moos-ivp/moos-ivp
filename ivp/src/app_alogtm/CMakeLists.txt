#--------------------------------------------------------
# The CMakeLists.txt for:                          alogtm
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

SET(SRC main.cpp TGrepHandler.cpp)

ADD_EXECUTABLE(alogtm ${SRC})
   
TARGET_LINK_LIBRARIES(alogtm
  mbutil
  logutils
  ${SYSTEM_LIBS})

