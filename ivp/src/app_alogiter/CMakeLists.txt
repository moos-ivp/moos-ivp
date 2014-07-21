#--------------------------------------------------------
# The CMakeLists.txt for:                        alogiter
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

SET(SRC main.cpp IterHandler.cpp)

ADD_EXECUTABLE(alogiter ${SRC})
   
TARGET_LINK_LIBRARIES(alogiter
  mbutil
  logutils
  ${SYSTEM_LIBS})

