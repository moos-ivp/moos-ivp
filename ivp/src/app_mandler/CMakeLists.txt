#--------------------------------------------------------
# The CMakeLists.txt for:                     app_mandler
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
   main.cpp
)
 
ADD_EXECUTABLE(mandler ${SRC})
     
TARGET_LINK_LIBRARIES(mandler
  manifest
  apputil
  mbutil
  ${SYSTEM_LIBS})

