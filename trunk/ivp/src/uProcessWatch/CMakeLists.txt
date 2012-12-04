#--------------------------------------------------------
# The CMakeLists.txt for:                   uProcessWatch
# Author(s):                                Mike Benjamin
#--------------------------------------------------------

# Set System Specific Libraries
if (${WIN32})
  SET(SYSTEM_LIBS
    wsock32)
else (${WIN32})
  SET(SYSTEM_LIBS
    pthread)
endif (${WIN32})

SET(SRC 
  ProcessWatch.cpp 
  ProcessWatch_Info.cpp
  main.cpp)

ADD_EXECUTABLE(uProcessWatch ${SRC})
 
TARGET_LINK_LIBRARIES(uProcessWatch 
  ${MOOS_LIBRARIES}
  apputil
  mbutil
  ${SYSTEM_LIBS})

