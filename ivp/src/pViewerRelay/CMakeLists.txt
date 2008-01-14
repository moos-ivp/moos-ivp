#--------------------------------------------------------
# The CMakeLists.txt for:                    pViewerRelay
# Author(s):                                Mike Benjamin
#--------------------------------------------------------

FILE(GLOB SRC *.cpp)

ADD_EXECUTABLE(pViewerRelay ${SRC})

TARGET_LINK_LIBRARIES(pViewerRelay 
MOOS
MOOSGen
MOOSUtility
mbutil
m
pthread)
