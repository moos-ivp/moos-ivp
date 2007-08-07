PROJECT(IVP_PBEARINGSSIM)

FILE(GLOB SRC *.cpp)

ADD_EXECUTABLE(pBearingsSim ${SRC})

TARGET_LINK_LIBRARIES(pBearingsSim
   MOOS
   MOOSGen
   opt
   m
   pthread
   )
