PROJECT(IVP_APP_PEVAL)

FILE(GLOB SRC *.cpp)

ADD_EXECUTABLE(peval ${SRC})

TARGET_LINK_LIBRARIES(peval
   navplot
   mbutil
   geometry
   m
   )
