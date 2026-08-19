#--------------------------------------------------------
# The CMakeLists.txt for:                  lib_marine_pid
# Author(s):                                Mike Benjamin
#--------------------------------------------------------

SET(SRC
   PIDEngine.cpp
   ScalarPID.cpp
)

# Build Library
IVP_ADD_LIBRARY(marine_pid ${SRC})
TARGET_LINK_LIBRARIES(marine_pid
  PUBLIC mbutil
  PRIVATE geometry MOOS::MOOS)
