#--------------------------------------------------------
# The CMakeLists.txt for:                        alogsort
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

SET(SRC main.cpp SortHandler.cpp)

ADD_EXECUTABLE(alogsort ${SRC})
   
TARGET_LINK_LIBRARIES(alogsort
  mbutil
  logutils
  ${SYSTEM_LIBS})

