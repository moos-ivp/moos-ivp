#--------------------------------------------------------
# The CMakeLists.txt for:                       alogsplit
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

SET(SRC main.cpp)

ADD_EXECUTABLE(alogsplit ${SRC})
   
TARGET_LINK_LIBRARIES(alogsplit
  logutils
  mbutil
  ${SYSTEM_LIBS})

