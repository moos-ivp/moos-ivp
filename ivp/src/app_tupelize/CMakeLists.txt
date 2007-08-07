PROJECT(IVP_APP_TUPELIZE)

FILE(GLOB SRC *.cpp)

ADD_EXECUTABLE(tupelize ${SRC})

TARGET_LINK_LIBRARIES(tupelize
   mbutil
   ivpbuild
   ivpcore
   m
   )
