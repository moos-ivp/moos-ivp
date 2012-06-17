#--------------------------------------------------------
# The CMakeLists.txt for:                     lib_genutil
# Author(s):                             Christian Convey
#--------------------------------------------------------

FILE(GLOB SRC *.cpp)

FILE(GLOB HEADERS *.h)

# Build Library
ADD_LIBRARY(genutil ${SRC})

