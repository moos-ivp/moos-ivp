PROJECT(IVP_APP_GRIDGEN)

FILE(GLOB SRC *.cpp)

ADD_EXECUTABLE(gridgen ${SRC})

TARGET_LINK_LIBRARIES(gridgen
   ivpbuild
   ivpcore
   geometry
   mbutil
   m
   )
