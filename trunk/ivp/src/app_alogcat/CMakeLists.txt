#--------------------------------------------------------
# The CMakeLists.txt for:                         alogcat
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
  ALogCatHandler.cpp)
  
ADD_EXECUTABLE(alogcat ${SRC}	)
   
TARGET_LINK_LIBRARIES(alogcat
  logutils
  mbutil
  ${SYSTEM_LIBS})


