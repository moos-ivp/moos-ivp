PROJECT(IVP_PFUNCTIONSENDER)

FILE(GLOB SRC *.cpp)

ADD_EXECUTABLE(pFunctionSender ${SRC})

TARGET_LINK_LIBRARIES(pFunctionSender
   MOOS
   MOOSGen
   ivpbuild
   mbutil
   ivpcore
   m
   pthread
   )
