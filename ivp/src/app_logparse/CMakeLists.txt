FILE(GLOB SRC *.cpp)

ADD_EXECUTABLE(logparse ${SRC})

TARGET_LINK_LIBRARIES(logparse 
   mbutil
   m)
