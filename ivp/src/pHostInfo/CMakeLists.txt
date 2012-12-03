#--------------------------------------------------------
# The CMakeLists.txt for:                       pHostInfo
# Author(s):                                Mike Benjamin
#--------------------------------------------------------

SET(SRC
  HostInfo.cpp
  HostInfo_Info.cpp
  main.cpp
)

ADD_EXECUTABLE(pHostInfo ${SRC})

TARGET_LINK_LIBRARIES(pHostInfo
   MOOS
   mbutil
   ufield
   apputil
   m
   pthread)

