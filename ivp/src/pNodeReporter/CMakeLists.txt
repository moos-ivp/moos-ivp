#--------------------------------------------------------
# The CMakeLists.txt for:                   pNodeReporter
# Author(s):                                Mike Benjamin
#--------------------------------------------------------

SET(SRC
   NodeReporter.cpp
   main.cpp
)

ADD_EXECUTABLE(pNodeReporter ${SRC})
   
TARGET_LINK_LIBRARIES(pNodeReporter
   MOOS
   MOOSGen
   MOOSUtility
   mbutil
   m
   pthread)
