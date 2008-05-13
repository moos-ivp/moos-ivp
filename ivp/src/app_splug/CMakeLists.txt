#--------------------------------------------------------
# The CMakeLists.txt for:                       app_splug
# Author(s):                                Mike Benjamin
#--------------------------------------------------------

SET(SRC
   Expander.cpp
   main.cpp
)
 
ADD_EXECUTABLE(splug ${SRC})
     
TARGET_LINK_LIBRARIES(splug
  mbutil
  m)
