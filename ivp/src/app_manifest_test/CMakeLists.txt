#--------------------------------------------------------
# The CMakeLists.txt for:               app_manifest_test
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
  ManifestTester.cpp 
)
 
ADD_EXECUTABLE(manifest_test ${SRC})
     
TARGET_LINK_LIBRARIES(manifest_test
  manifest
  apputil
  mbutil
  ${SYSTEM_LIBS})

