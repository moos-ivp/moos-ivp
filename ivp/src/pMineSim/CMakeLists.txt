PROJECT(IVP_PMINESIM)

FILE(GLOB SRC *.cpp)

ADD_EXECUTABLE(pMineSim ${SRC})

TARGET_LINK_LIBRARIES(pMineSim
   MOOS
   MOOSGen
   geometry
   mbutil
   m
   pthread
   )
