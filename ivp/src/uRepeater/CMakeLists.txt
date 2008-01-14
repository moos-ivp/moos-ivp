#--------------------------------------------------------
# The CMakeLists.txt for:                   uProcessWatch
# Author(s):                                Mike Benjamin
#--------------------------------------------------------

FILE(GLOB SRC *.cpp)

ADD_EXECUTABLE(uRepeater ${SRC})
 
TARGET_LINK_LIBRARIES(uRepeater
   MOOS
   MOOSGen
   mbutil
   m
   pthread)
