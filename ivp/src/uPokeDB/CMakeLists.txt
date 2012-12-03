#--------------------------------------------------------
# The CMakeLists.txt for:                         uPokeDB
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

ADD_EXECUTABLE(uPokeDB ${SRC})

TARGET_LINK_LIBRARIES(uPokeDB
   ${MOOS_LIBRARIES}
   mbutil
   ${SYSTEM_LIBS})

