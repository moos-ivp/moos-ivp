PROJECT(IVP_UPROCESSWATCH)

FILE(GLOB SRC *.cpp)

ADD_EXECUTABLE(uProcessWatch ${SRC})

TARGET_LINK_LIBRARIES(uProcessWatch
   MOOS
   MOOSGen
   mbutil
   m
   pthread
   )
