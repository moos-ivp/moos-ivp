#--------------------------------------------------------
# The CMakeLists.txt for:                app_manifest_loc
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
  LOCTester.cpp 
)
 
ADD_EXECUTABLE(manifest_loc ${SRC})
     
TARGET_LINK_LIBRARIES(manifest_loc
  manifest
  apputil
  mbutil
  ${SYSTEM_LIBS})

