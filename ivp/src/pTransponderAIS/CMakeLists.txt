#--------------------------------------------------------
# The CMakeLists.txt for:                 pTransponderAIS
# Author(s):                                Mike Benjamin
#--------------------------------------------------------

SET(SRC
   TransponderAIS.cpp
   mainTransponderAIS.cpp
)

ADD_EXECUTABLE(pTransponderAIS ${SRC})
   
TARGET_LINK_LIBRARIES(pTransponderAIS 
   MOOS
   MOOSGen
   MOOSUtility
   mbutil
   m
   pthread)
