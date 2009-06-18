#--------------------------------------------------------
# The CMakeLists.txt for:                      app_nsplug
# Author(s):                                Mike Benjamin
#--------------------------------------------------------

SET(SRC
   Expander.cpp
   main.cpp
)
 
ADD_EXECUTABLE(nsplug ${SRC})
     
TARGET_LINK_LIBRARIES(nsplug
  mbutil
  m)
