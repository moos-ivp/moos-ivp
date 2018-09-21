#--------------------------------------------------------
# The CMakeLists.txt for:                        alogclip
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

SET(SRC 
  main.cpp 
  ALogClipHandler.cpp
  ALogClipper.cpp )

ADD_EXECUTABLE(alogclip ${SRC})
   
TARGET_LINK_LIBRARIES(alogclip
  mbutil
  ${SYSTEM_LIBS})


