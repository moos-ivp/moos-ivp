#--------------------------------------------------------
# The CMakeLists.txt for:                        alogscan
# Author(s):                                Mike Benjamin
#--------------------------------------------------------

FILE(GLOB SRC *.cpp)

ADD_EXECUTABLE(alogscan ${SRC})
   
TARGET_LINK_LIBRARIES(alogscan
  mbutil
  logutils
  m)
