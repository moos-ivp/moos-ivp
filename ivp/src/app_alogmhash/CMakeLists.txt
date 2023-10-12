#--------------------------------------------------------
# The CMakeLists.txt for:                       alogmhash
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

SET(SRC main.cpp MHashReporter.cpp)

ADD_EXECUTABLE(alogmhash ${SRC})
   
TARGET_LINK_LIBRARIES(alogmhash
  logutils
  mbutil
  ${SYSTEM_LIBS})

