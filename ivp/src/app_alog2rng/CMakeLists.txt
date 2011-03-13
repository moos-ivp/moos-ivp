#--------------------------------------------------------
# The CMakeLists.txt for:                        alog2rng
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

SET(SRC main.cpp Alog2Range.cpp)

ADD_EXECUTABLE(alog2rng ${SRC})
   
TARGET_LINK_LIBRARIES(alog2rng
  mbutil
  logutils
  navplot
  ${SYSTEM_LIBS})

# Install Targets
INSTALL(TARGETS alog2rng
  RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin
)

