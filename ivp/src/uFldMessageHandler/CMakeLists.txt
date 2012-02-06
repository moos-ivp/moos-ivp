#--------------------------------------------------------
# The CMakeLists.txt for:              uFldMessageHandler
# Author(s):                                Mike Benjamin
#--------------------------------------------------------

FILE(GLOB SRC *.cpp)

ADD_EXECUTABLE(uFldMessageHandler ${SRC})

TARGET_LINK_LIBRARIES(uFldMessageHandler
   MOOS
   MOOSGen
   mbutil
   ufield
   m
   pthread)

