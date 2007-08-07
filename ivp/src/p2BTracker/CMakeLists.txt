PROJECT(IVP_P2BTRACKER)

FILE(GLOB SRC *.cpp)

ADD_EXECUTABLE(p2BTracker ${SRC})

TARGET_LINK_LIBRARIES(p2BTracker
   MOOS
   MOOSGen
   opt
   m
   pthread
   )
