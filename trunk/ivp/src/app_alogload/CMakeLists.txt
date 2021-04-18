#--------------------------------------------------------
# The CMakeLists.txt for:                        alogload
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

SET(SRC main.cpp LoadReporter.cpp)

ADD_EXECUTABLE(alogload ${SRC})
   
TARGET_LINK_LIBRARIES(alogload
  logutils
  helmivp
  behaviors
  ivpbuild
  ivpcore
  ivpsolve
  mbutil
  ${SYSTEM_LIBS})

