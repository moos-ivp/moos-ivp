#--------------------------------------------------------
# The CMakeLists.txt for:                    pViewerRelay
# Author(s):                                Mike Benjamin
#--------------------------------------------------------

SET(SRC
   ViewerRelay.cpp
   ViewerRelayMain.cpp
)

ADD_EXECUTABLE(pViewerRelay ${SRC})

TARGET_LINK_LIBRARIES(pViewerRelay 
MOOS
MOOSGen
MOOSUtility
mbutil
m
pthread)
