#--------------------------------------------------------
# The CMakeLists.txt for:                       pHostInfo
# Author(s):                                Mike Benjamin
#--------------------------------------------------------

FILE(GLOB SRC *.cpp)

ADD_EXECUTABLE(pHostInfo ${SRC})

TARGET_LINK_LIBRARIES(pHostInfo
   MOOS
   MOOSGen
   mbutil
   ufield
   m
   pthread)

