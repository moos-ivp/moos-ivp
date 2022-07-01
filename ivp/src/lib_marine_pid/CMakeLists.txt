#--------------------------------------------------------
# The CMakeLists.txt for:                  lib_marine_pid
# Author(s):                                Mike Benjamin
#--------------------------------------------------------

SET(SRC
   PIDEngine.cpp
   ScalarPID.cpp
)

# Build Library
ADD_LIBRARY(marine_pid ${SRC})
