#--------------------------------------------------------
# The CMakeLists.txt for:                       iDiffThrust
# Author(s):                                Mike Benjamin
#--------------------------------------------------------

FILE(GLOB SRC *.cpp)

ADD_EXECUTABLE(iDiffThrust ${SRC})

TARGET_LINK_LIBRARIES(iDiffThrust
   ${MOOS_LIBRARIES}
   genutil
   mbutil
   m
   pthread)

