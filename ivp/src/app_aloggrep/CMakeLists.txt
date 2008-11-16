#--------------------------------------------------------
# The CMakeLists.txt for:                        aloggrep
# Author(s):                                Mike Benjamin
#--------------------------------------------------------

FILE(GLOB SRC *.cpp)

ADD_EXECUTABLE(aloggrep ${SRC})
   
TARGET_LINK_LIBRARIES(aloggrep
  mbutil
  logutils
  m)
