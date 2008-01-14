#--------------------------------------------------------
# The CMakeLists.txt for:                        pEchoVar
# Author(s):                                Mike Benjamin
#--------------------------------------------------------

FILE(GLOB SRC *.cpp)

ADD_EXECUTABLE(pEchoVar ${SRC})

TARGET_LINK_LIBRARIES(pEchoVar 
MOOS
MOOSGen
mbutil
m
pthread)
