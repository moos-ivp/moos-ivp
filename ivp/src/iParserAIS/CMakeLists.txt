#--------------------------------------------------------
# The CMakeLists.txt for:                      iParserAIS
# Author(s):                                   Phong TRan
#--------------------------------------------------------

FILE(GLOB SRC *.cpp)

ADD_EXECUTABLE(iParserAIS ${SRC})

TARGET_LINK_LIBRARIES(iParserAIS 
MOOS
MOOSGen
MOOSUtility
mbutil
ais
m
pthread)
