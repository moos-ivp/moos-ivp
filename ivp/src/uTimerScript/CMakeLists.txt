#--------------------------------------------------------
# The CMakeLists.txt for:                    uTimerScript
# Author(s):                                Mike Benjamin
#--------------------------------------------------------

# Set System Specific Libraries
if (${WIN32})
  SET(SYSTEM_LIBS
    wsock32)
else (${WIN32})
  SET(SYSTEM_LIBS
    m
    pthread)
endif (${WIN32})

SET(SRC TS_MOOSApp.cpp RandomVariable main.cpp)

ADD_EXECUTABLE(uTimerScript ${SRC})

TARGET_LINK_LIBRARIES(uTimerScript
   MOOS
   MOOSGen
   mbutil
   logic
   ${SYSTEM_LIBS})

