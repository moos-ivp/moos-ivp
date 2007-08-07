PROJECT(IVP_PHELMIVP)

FILE(GLOB SRC *.cpp)

ADD_EXECUTABLE(pHelmIvP ${SRC})

TARGET_LINK_LIBRARIES(pHelmIvP
   MOOS
   MOOSGen
   helmivp
   behaviors-marine
   behaviors-sandbox
   bhvutil	
   behaviors
   mbutil 
   ivpbuild
   ivpcore 
   geometry
   logic
   m
   pthread
   )
