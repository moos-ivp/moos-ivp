#--------------------------------------------------------
# The CMakeLists.txt for:                        alogscan
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

SET(SRC main.cpp ScanHandler.cpp)

ADD_EXECUTABLE(alogscan ${SRC})
   
TARGET_LINK_LIBRARIES(alogscan
  mbutil
  logutils
  ${SYSTEM_LIBS})

