#--------------------------------------------------------
# The CMakeLists.txt for:                  pSafetyRetreat
# Author(s):                                Mike Benjamin
#--------------------------------------------------------

FILE(GLOB SRC *.cpp)

ADD_EXECUTABLE(pSafetyRetreat ${SRC})

TARGET_LINK_LIBRARIES(pSafetyRetreat
   MOOS
   MOOSGen
   mbutil
   geometry
   m
   pthread)

