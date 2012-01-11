#--------------------------------------------------------
# The CMakeLists.txt for:                   uFldNodeComms
# Author(s):                                Mike Benjamin
#--------------------------------------------------------
 
FILE(GLOB SRC *.cpp)

ADD_EXECUTABLE(uFldNodeComms ${SRC})

TARGET_LINK_LIBRARIES(uFldNodeComms
   MOOS
   MOOSGen
   mbutil
   contacts
   ufield
   geometry
   m
   pthread)

