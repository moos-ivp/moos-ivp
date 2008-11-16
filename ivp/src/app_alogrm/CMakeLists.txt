#--------------------------------------------------------
# The CMakeLists.txt for:                          alogrm
# Author(s):                                Mike Benjamin
#--------------------------------------------------------

FILE(GLOB SRC *.cpp)

ADD_EXECUTABLE(alogrm ${SRC})
   
TARGET_LINK_LIBRARIES(alogrm
  mbutil
  logutils
  m)
