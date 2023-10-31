#--------------------------------------------------------
# The CMakeLists.txt for:                        alogeval
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

SET(SRC main.cpp ALogEvaluator.cpp)

ADD_EXECUTABLE(alogeval ${SRC})
   
TARGET_LINK_LIBRARIES(alogeval
  evalutil
  apputil
  geometry
  logic
  mbutil
  logutils
  ${SYSTEM_LIBS})

