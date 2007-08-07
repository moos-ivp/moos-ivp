PROJECT(IVP_IPWMCONTROLLER)

FILE(GLOB SRC *.cpp)

ADD_EXECUTABLE(iPWMController ${SRC})

TARGET_LINK_LIBRARIES(iPWMController
   MOOS
   MOOSGen
   anrp_util
   m
   pthread
   )
