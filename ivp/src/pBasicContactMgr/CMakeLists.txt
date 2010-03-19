#--------------------------------------------------------
# The CMakeLists.txt for:                pBasicContactMgr
# Author(s):                                Mike Benjamin
#--------------------------------------------------------

FILE(GLOB SRC *.cpp)

ADD_EXECUTABLE(pBasicContactMgr ${SRC})

TARGET_LINK_LIBRARIES(pBasicContactMgr
   MOOS
   MOOSGen
   mbutil
   geometry
   m
   pthread)

