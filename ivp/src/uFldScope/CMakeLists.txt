#--------------------------------------------------------
# The CMakeLists.txt for:                       uFldScope
# Author(s):                                Mike Benjamin
#--------------------------------------------------------

FILE(GLOB  SRC *.cpp)

ADD_EXECUTABLE(uFldScope ${SRC})

TARGET_LINK_LIBRARIES(uFldScope
   ${MOOS_LIBRARIES}
   mbutil
   apputil
   m
   pthread)

