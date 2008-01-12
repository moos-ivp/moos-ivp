#--------------------------------------------------------
# The CMakeLists.txt for:                       app_splug
# Author(s):                                Mike Benjamin
#--------------------------------------------------------

FILE(GLOB SRC *.cpp)
  
ADD_EXECUTABLE(splug ${SRC})
     
TARGET_LINK_LIBRARIES(splug
  mbutil
  m)
