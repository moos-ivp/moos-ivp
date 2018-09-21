#--------------------------------------------------------
# The CMakeLists.txt for:                        uQueryDB
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

FILE(GLOB SRC *.cpp)

ADD_EXECUTABLE(uQueryDB ${SRC})

TARGET_LINK_LIBRARIES(uQueryDB
   ${MOOS_LIBRARIES}
   mbutil
   logic
   ${SYSTEM_LIBS})

