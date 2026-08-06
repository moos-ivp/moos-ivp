#--------------------------------------------------------
# The CMakeLists.txt for:                     lib_genutil
# Author(s):                             Christian Convey
#--------------------------------------------------------

FILE(GLOB SRC *.cpp)

FILE(GLOB HEADERS *.h)

# Build Library
IVP_ADD_LIBRARY(genutil ${SRC})
TARGET_LINK_LIBRARIES(genutil
  PUBLIC MOOS::MOOS)
