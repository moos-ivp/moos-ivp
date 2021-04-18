#--------------------------------------------------------
# The CMakeLists.txt for:                          alogcd
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

SET(SRC main.cpp CollisionReporter.cpp)

ADD_EXECUTABLE(alogcd ${SRC})
   
TARGET_LINK_LIBRARIES(alogcd
  logutils
  mbutil
  ${SYSTEM_LIBS})

