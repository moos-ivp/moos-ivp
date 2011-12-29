#--------------------------------------------------------
# The CMakeLists.txt for:                     uMOOSClient
# Author(s):                                Mike Benjamin
#--------------------------------------------------------

FILE(GLOB SRC *.cpp)

ADD_EXECUTABLE(uMOOSClient ${SRC})


TARGET_LINK_LIBRARIES(uMOOSClient
   MOOS
   MOOSGen
   mbutil
   m
   pthread)

