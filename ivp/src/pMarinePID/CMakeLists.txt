PROJECT(IVP_PMARINEPID)

FILE(GLOB SRC *.cpp)

ADD_EXECUTABLE(pMarinePID ${SRC})

TARGET_LINK_LIBRARIES(pMarinePID
   MOOS
   MOOSGen
   mbutil
   geometry
   m
   pthread
   )
