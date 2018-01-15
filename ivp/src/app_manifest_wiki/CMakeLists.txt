#--------------------------------------------------------
# The CMakeLists.txt for:               app_manifest_wiki
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
  ManifestHandler.cpp 
)
 
ADD_EXECUTABLE(manifest_wiki ${SRC})
     
TARGET_LINK_LIBRARIES(manifest_wiki
  manifest
  apputil
  mbutil
  ${SYSTEM_LIBS})

