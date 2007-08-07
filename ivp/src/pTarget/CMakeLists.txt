PROJECT(IVP_PTARGET)

FILE(GLOB SRC *.cpp)

ADD_EXECUTABLE(pTarget ${SRC})

TARGET_LINK_LIBRARIES(pTarget
   MOOS
   MOOSGen
   geometry
   mbutil
   m
   pthread
   )
