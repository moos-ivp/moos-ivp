#--------------------------------------------------------
# The CMakeLists.txt for:                        aloghelm
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

SET(SRC main.cpp HelmReporter.cpp)

ADD_EXECUTABLE(aloghelm ${SRC})
   
TARGET_LINK_LIBRARIES(aloghelm
  logutils
  helmivp
  behaviors
  ivpbuild
  ivpcore
  ivpsolve
  mbutil
  ${SYSTEM_LIBS})

