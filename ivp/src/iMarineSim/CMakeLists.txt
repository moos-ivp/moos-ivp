PROJECT(IVP_IMARINESIM)

FILE(GLOB SRC *.cpp)

ADD_EXECUTABLE(iMarineSim ${SRC})

TARGET_LINK_LIBRARIES(iMarineSim
   MOOS
   MOOSGen
   mbutil
   geometry
   m
   pthread
   )
