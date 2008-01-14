#--------------------------------------------------------
# The CMakeLists.txt for:                  pDeployHandler
# Author(s):                                Mike Benjamin
#--------------------------------------------------------

FILE(GLOB SRC *.cpp)

ADD_EXECUTABLE(pDeployHandler ${SRC})

TARGET_LINK_LIBRARIES(pDeployHandler
   MOOS
   MOOSGen
   mbutil
   m
   pthread
   )