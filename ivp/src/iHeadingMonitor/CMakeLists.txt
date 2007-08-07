PROJECT(IVP_IHEADINGMONITOR)

FILE(GLOB SRC *.cpp)

ADD_EXECUTABLE(iHeadingMonitor ${SRC})

TARGET_LINK_LIBRARIES(iHeadingMonitor
   MOOS
   MOOSGen
   mbutil
   m
   pthread
   )
