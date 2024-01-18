#--------------------------------------------------------
# The CMakeLists.txt for:                   app_projfield
# Author(s):                                Mike Benjamin
#--------------------------------------------------------

FILE(GLOB SRC
  main.cpp
  FldProjector.cpp)

ADD_EXECUTABLE(projfield ${SRC})
   
TARGET_LINK_LIBRARIES(projfield
  geometry
  mbutil
  m)

