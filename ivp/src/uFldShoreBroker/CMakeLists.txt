#--------------------------------------------------------
# The CMakeLists.txt for:                 uFldShoreBroker
# Author(s):                                Mike Benjamin
#--------------------------------------------------------
 
FILE(GLOB SRC *.cpp)

ADD_EXECUTABLE(uFldShoreBroker ${SRC})

TARGET_LINK_LIBRARIES(uFldShoreBroker
   ${MOOS_LIBRARIES}
   mbutil
   apputil
   ufield
   m
   pthread)

