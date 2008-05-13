#--------------------------------------------------------
# The CMakeLists.txt for:                      iParserAIS
# Author(s):                                   Phong TRan
#--------------------------------------------------------

SET(SRC
   ParserAIS.cpp
   mainParserAIS.cpp
)

ADD_EXECUTABLE(iParserAIS ${SRC})

TARGET_LINK_LIBRARIES(iParserAIS 
MOOS
MOOSGen
MOOSUtility
mbutil
ais
m
pthread)
