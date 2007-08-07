PROJECT(IVP_IPNICOMPASS)

FILE(GLOB SRC *.cpp)

ADD_EXECUTABLE(iPNICompass ${SRC})

TARGET_LINK_LIBRARIES(iPNICompass
   MOOS
   MOOSGen
   anrp_util
   m
   pthread
   )
