#--------------------------------------------------------
# The CMakeLists.txt for:                        alogeff
# Author(s):                                Kyle Woerner
#--------------------------------------------------------

# Set System Specific Libraries
if (${WIN32})
  SET(SYSTEM_LIBS
    wsock32)
else (${WIN32})
  SET(SYSTEM_LIBS
    m)
endif (${WIN32})

SET(SRC main.cpp EffHandler.cpp)

ADD_EXECUTABLE(alogeff ${SRC})
   
TARGET_LINK_LIBRARIES(alogeff
  mbutil
  logutils
  ${SYSTEM_LIBS})

