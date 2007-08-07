PROJECT(IVP_PCLUSTERMANAGER)

FILE(GLOB SRC *.cpp)

ADD_EXECUTABLE(pClusterManager ${SRC})

TARGET_LINK_LIBRARIES(pClusterManager
   MOOS
   MOOSGen
   mbutil
   geometry
   m
   pthread
   )
