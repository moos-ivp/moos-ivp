#--------------------------------------------------------
# The CMakeLists.txt for:                    uTimerScript
# Author(s):                                Mike Benjamin
#--------------------------------------------------------

FILE(GLOB SRC *.cpp)

ADD_EXECUTABLE(uTimerScript ${SRC})

TARGET_LINK_LIBRARIES(uTimerScript
   MOOS
   MOOSGen
   m
   mbutil
   pthread)

