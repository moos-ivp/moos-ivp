#--------------------------------------------------------
# The CMakeLists.txt for:                        alogfilt
# Author(s):                                Mike Benjamin
#--------------------------------------------------------

FILE(GLOB SRC *.cpp)

ADD_EXECUTABLE(alogfilt ${SRC})
   
TARGET_LINK_LIBRARIES(alogfilt
  mbutil
  logutils
  m)
