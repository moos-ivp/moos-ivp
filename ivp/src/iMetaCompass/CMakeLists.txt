PROJECT(IVP_IMETACOMPASS)

FILE(GLOB SRC *.cpp)

ADD_EXECUTABLE(iMetaCompass ${SRC})

TARGET_LINK_LIBRARIES(iMetaCompass
   MOOS
   MOOSGen
   mbutil
   m
   pthread
   )
