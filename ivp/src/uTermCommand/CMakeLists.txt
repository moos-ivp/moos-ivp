#--------------------------------------------------------
# The CMakeLists.txt for:                    uTermCommand
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
  TermCommand.cpp
  TermCommandMain.cpp)

ADD_EXECUTABLE(uTermCommand ${SRC})

TARGET_LINK_LIBRARIES(uTermCommand 
  MOOS
  MOOSGen
  mbutil
  genutil
  ${SYSTEM_LIBS})

