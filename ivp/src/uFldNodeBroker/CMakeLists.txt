#--------------------------------------------------------
# The CMakeLists.txt for:                  uFldNodeBroker
# Author(s):                                Mike Benjamin
#--------------------------------------------------------

FILE(GLOB SRC *.cpp)

ADD_EXECUTABLE(uFldNodeBroker ${SRC})

TARGET_LINK_LIBRARIES(uFldNodeBroker
   MOOS
   MOOSGen
   mbutil
   ufield
   m
   pthread)

