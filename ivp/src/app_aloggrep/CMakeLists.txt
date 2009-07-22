#--------------------------------------------------------
# The CMakeLists.txt for:                        aloggrep
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

FILE(GLOB SRC *.cpp)

ADD_EXECUTABLE(aloggrep ${SRC})
   
TARGET_LINK_LIBRARIES(aloggrep
  mbutil
  logutils
  ${SYSTEM_LIBS})
