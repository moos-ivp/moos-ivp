#--------------------------------------------------------
# The CMakeLists.txt for:                            uMAC
# Author(s):                                Mike Benjamin   
#--------------------------------------------------------

# FILE(GLOB SRC *.cpp)

SET(SRC
  AppCastMonitor.cpp
  AppCastMonitor_Info.cpp
  main.cpp
)

ADD_EXECUTABLE(uMAC ${SRC})

TARGET_LINK_LIBRARIES(uMAC
   ${MOOS_LIBRARIES}
   genutil
   mbutil
   apputil
   m
   pthread)

