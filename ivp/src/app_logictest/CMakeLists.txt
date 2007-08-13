FILE(GLOB SRC *.cpp)

ADD_EXECUTABLE(ltest ${SRC})

TARGET_LINK_LIBRARIES(ltest 
   logic
   mbutil
   m)
