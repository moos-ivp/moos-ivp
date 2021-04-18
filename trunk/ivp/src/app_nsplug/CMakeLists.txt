#--------------------------------------------------------
# The CMakeLists.txt for:                      app_nsplug
# Author(s):                                Mike Benjamin
#--------------------------------------------------------

# Set System Specific Libraries
if (${WIN32})
  SET(SYSTEM_LIBS
    )
else (${WIN32})
  SET(SYSTEM_LIBS
    m)
endif (${WIN32})

SET(SRC
   Expander.cpp
   Expander_Info.cpp
   main.cpp
)
 
ADD_EXECUTABLE(nsplug ${SRC})
     
TARGET_LINK_LIBRARIES(nsplug
  mbutil
  ${SYSTEM_LIBS})

