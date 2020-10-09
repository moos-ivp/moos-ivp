#--------------------------------------------------------
# The CMakeLists.txt for:                         alogbin
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

SET(SRC main.cpp LogBinHandler.cpp)

ADD_EXECUTABLE(alogbin ${SRC})
   
TARGET_LINK_LIBRARIES(alogbin
  mbutil
  logutils
  ${SYSTEM_LIBS})

