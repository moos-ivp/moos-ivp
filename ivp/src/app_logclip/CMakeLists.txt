FILE(GLOB SRC *.cpp)

ADD_EXECUTABLE(logclip ${SRC})

TARGET_LINK_LIBRARIES(logclip 
   mbutil
   m)
