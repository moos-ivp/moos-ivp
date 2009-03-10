#--------------------------------------------------------
# The CMakeLists.txt for:                    app_domquery
# Author(s):                   Benjamin, Schmidt, Leonard
#--------------------------------------------------------

SET(SRC
   DomQuery.cpp
   main.cpp
)
 
ADD_EXECUTABLE(domquery ${SRC})
     
TARGET_LINK_LIBRARIES(domquery
  mbutil
  m)
