#--------------------------------------------------------
# The CMakeLists.txt for:                     uSimCurrent
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

SET(SRC
   USC_MOOSApp.cpp
   USC_Info.cpp
   main.cpp
)

ADD_EXECUTABLE(uSimCurrent ${SRC})

TARGET_LINK_LIBRARIES(uSimCurrent
  MOOS
  MOOSGen
  MOOSUtility
  mbutil
  geometry 
  ${SYSTEM_LIBS})

# Install Targets
INSTALL(TARGETS uSimCurrent
  RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin
)

