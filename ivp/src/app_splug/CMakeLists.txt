#--------------------------------------------------------
# The CMakeLists.txt for:                       app_splug
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
   Expander.cpp
   main.cpp
)
 
ADD_EXECUTABLE(splug ${SRC})
     
TARGET_LINK_LIBRARIES(splug
  mbutil
  ${SYSTEM_LIBS})
