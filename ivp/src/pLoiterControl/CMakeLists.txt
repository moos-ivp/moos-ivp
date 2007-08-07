PROJECT(IVP_PLOITERCONTROL)

FILE(GLOB SRC *.cpp)

ADD_EXECUTABLE(pLoiterControl ${SRC})

TARGET_LINK_LIBRARIES(pLoiterControl
   MOOS
   MOOSGen
   mbutil
   geometry
   m
   pthread
   )
