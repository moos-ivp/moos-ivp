PROJECT(IVP_UTMS)

FILE(GLOB SRC *.cpp)

ADD_EXECUTABLE(uTMS ${SRC})

TARGET_LINK_LIBRARIES(uTMS
   MOOS
   MOOSGen
   genutil
   mbutil
   m
   pthread
   )
