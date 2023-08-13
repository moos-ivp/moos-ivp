#--------------------------------------------------------
# The CMakeLists.txt for:                        alogpick
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

SET(SRC main.cpp PickHandler.cpp)

ADD_EXECUTABLE(alogpick ${SRC})
   
TARGET_LINK_LIBRARIES(alogpick
  apputil
  mbutil
  logutils
  ${SYSTEM_LIBS})

