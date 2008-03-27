#--------------------------------------------------------
# The CMakeLists.txt for:                   iPlaybackAIS
# Author(s):                                Phong Tran
#--------------------------------------------------------

FILE(GLOB SRC *.cpp)

ADD_EXECUTABLE(iPlaybackAIS ${SRC})

TARGET_LINK_LIBRARIES(iPlaybackAIS 
MOOS
MOOSGen
MOOSUtility
mbutil
m
pthread)
