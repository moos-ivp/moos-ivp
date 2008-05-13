#--------------------------------------------------------
# The CMakeLists.txt for:                       app_etest
# Author(s):                                Mike Benjamin
#--------------------------------------------------------

SET(SRC
   TestEncodeMK.cpp
   main.cpp
)
  
ADD_EXECUTABLE(etest ${SRC})
     
TARGET_LINK_LIBRARIES(etest
  mbutil
  ivpbuild
  ivpcore
  m)

