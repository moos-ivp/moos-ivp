PROJECT(IVP_APP_TEST)

FILE(GLOB SRC *.cpp)

ADD_EXECUTABLE(stest ${SRC})

TARGET_LINK_LIBRARIES(stest
   ivpbuild
   mbutil
   geometry
   ivpcore
   logic
   m
   )
