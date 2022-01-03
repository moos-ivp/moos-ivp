#--------------------------------------------------------
# The CMakeLists.txt for:                         alogavg
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

SET(SRC main.cpp AvgHandler.cpp ExpEntry.cpp)

ADD_EXECUTABLE(alogavg ${SRC})
   
TARGET_LINK_LIBRARIES(alogavg
  apputil
  mbutil
  logutils
  ${SYSTEM_LIBS})

