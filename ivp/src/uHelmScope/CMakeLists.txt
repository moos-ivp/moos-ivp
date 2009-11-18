#--------------------------------------------------------
# The CMakeLists.txt for:                      uHelmScope
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
  HelmScope.cpp
  IterBlockHelm.cpp
  IterBlockBHV.cpp
  IterBlockPosts.cpp
  main.cpp
)

ADD_EXECUTABLE(uHelmScope ${SRC})

TARGET_LINK_LIBRARIES(uHelmScope 
  MOOS
  MOOSGen
  genutil
  mbutil 
  ${SYSTEM_LIBS})

