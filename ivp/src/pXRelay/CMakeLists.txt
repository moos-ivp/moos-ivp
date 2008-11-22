#--------------------------------------------------------
# The CMakeLists.txt for:                         pXRelay
# Author(s):                                Mike Benjamin
#--------------------------------------------------------

SET(SRC
   Relayer.cpp  
   main.cpp
)  

ADD_EXECUTABLE(pXRelay ${SRC}	)

TARGET_LINK_LIBRARIES(pXRelay
   MOOS
   MOOSGen
   mbutil
   m
   pthread)
