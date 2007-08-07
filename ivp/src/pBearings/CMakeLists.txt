PROJECT(IVP_PBEARINGS)

FILE(GLOB SRC *.cpp)

ADD_EXECUTABLE(pBearings ${SRC})

TARGET_LINK_LIBRARIES(pBearings
   MOOS
   MOOSGen
   opt
   newmat
   m
   pthread
   )
