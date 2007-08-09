PROJECT(IVP_UTERMCOMMAND)

FILE(GLOB SRC *.cpp)

ADD_EXECUTABLE(uTermCommand ${SRC})

TARGET_LINK_LIBRARIES(uTermCommand
   MOOS
   MOOSGen
   MOOSUtility
   mbutil
   genutil
   m
   pthread
   )
