#--------------------------------------------------------
# The CMakeLists.txt for:                    lib_manifest
# Author(s):                                Mike Benjamin
#--------------------------------------------------------

SET(SRC 
  Manifest.cpp
  ManifestToWikiPage.cpp
  Populator_Manifest.cpp)

ADD_LIBRARY(manifest ${SRC})
