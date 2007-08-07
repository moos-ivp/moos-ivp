PROJECT(IVP_PTRANSPONDERAIS)

FILE(GLOB SRC *.cpp)

ADD_EXECUTABLE(pTransponderAIS ${SRC})

TARGET_LINK_LIBRARIES(pTransponderAIS
   MOOS
   MOOSGen
   mbutil
   m
   pthread
   )
