#--------------------------------------------------------
# The CMakeLists.txt for:                     lib_genutil
# Author(s):                             Christian Convey
#--------------------------------------------------------

FILE(GLOB SRC *.cpp)
IF(IVP_BUILD_HELM_LIBS_ONLY)
  # The helm only uses these; the rest runs MOOS apps
  SET(SRC fileutil.cpp stringutil.cpp)
ENDIF()

FILE(GLOB HEADERS *.h)

# Build Library
IVP_ADD_LIBRARY(genutil ${SRC})
IF(NOT IVP_BUILD_HELM_LIBS_ONLY)
  TARGET_LINK_LIBRARIES(genutil
    PUBLIC MOOS::MOOS)
ENDIF()
