#--------------------------------------------------------
# The CMakeLists.txt for:                    lib_manifest
# Author(s):                                Mike Benjamin
#--------------------------------------------------------

SET(SRC 
  Manifest.cpp
  ManifestSet.cpp
  Populator_ManifestSet.cpp)

IVP_ADD_LIBRARY(manifest ${SRC})
TARGET_LINK_LIBRARIES(manifest
  PRIVATE mbutil)
