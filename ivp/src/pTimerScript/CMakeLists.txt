#--------------------------------------------------------
# The CMakeLists.txt for:                    pTimerScript
# Author(s):                                Mike Benjamin
#--------------------------------------------------------

FILE(GLOB SRC *.cpp)

ADD_EXECUTABLE(pTimerScript ${SRC})

TARGET_LINK_LIBRARIES(pTimerScript
   MOOS
   MOOSGen
   m
   mbutil
   pthread)

