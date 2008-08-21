#--------------------------------------------------------
# The CMakeLists.txt for:                        aloggrab
# Author(s):                                Mike Benjamin
#--------------------------------------------------------

FILE(GLOB SRC *.cpp)

ADD_EXECUTABLE(aloggrab ${SRC})
   
TARGET_LINK_LIBRARIES(aloggrab
  mbutil
  logutils
  m)
