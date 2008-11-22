#--------------------------------------------------------
# The CMakeLists.txt for:                         uPokeDB
# Author(s):                                Mike Benjamin
#--------------------------------------------------------

FILE(GLOB SRC *.cpp)

ADD_EXECUTABLE(uPokeDB ${SRC})

TARGET_LINK_LIBRARIES(uPokeDB
   MOOS
   MOOSGen
   mbutil
   m
   pthread)


