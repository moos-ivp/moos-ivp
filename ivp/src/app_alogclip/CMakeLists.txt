#--------------------------------------------------------
# The CMakeLists.txt for:                        alogclip
# Author(s):                                Mike Benjamin
#--------------------------------------------------------

FILE(GLOB SRC *.cpp)

ADD_EXECUTABLE(alogclip ${SRC})
   
TARGET_LINK_LIBRARIES(alogclip
  mbutil
  m)
