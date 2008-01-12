#--------------------------------------------------------
# The CMakeLists.txt for:                       app_etest
# Author(s):                                Mike Benjamin
#--------------------------------------------------------

FILE(GLOB SRC *.cpp)
  
ADD_EXECUTABLE(etest ${SRC})
     
TARGET_LINK_LIBRARIES(etest
  mbutil
  ivpbuild
  ivpcore
  m)

