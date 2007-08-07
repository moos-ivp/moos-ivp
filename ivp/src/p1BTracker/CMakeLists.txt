PROJECT(IVP_P1BTRACKER)

FILE(GLOB SRC *.cpp)

ADD_EXECUTABLE(p1BTracker ${SRC})

TARGET_LINK_LIBRARIES(p1BTracker
   MOOS
   MOOSGen
   opt
   m
   pthread
   )
