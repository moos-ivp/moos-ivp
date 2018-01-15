#--------------------------------------------------------
# The CMakeLists.txt for:                    lib_manifest
# Author(s):                                Mike Benjamin
#--------------------------------------------------------

SET(SRC 
  Manifest.cpp
  ManifestSet.cpp
  Populator_ManifestSet.cpp)

ADD_LIBRARY(manifest ${SRC})

