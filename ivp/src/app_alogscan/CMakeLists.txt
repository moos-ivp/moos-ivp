FILE(GLOB SRC *.cpp)

ADD_EXECUTABLE(alogscan ${SRC})
   
TARGET_LINK_LIBRARIES(alogscan
  mbutil
  m)
