#--------------------------------------------------------
# The CMakeLists.txt for:         testBearingMinMaxToPoly
# Author(s):                                Mike Benjamin
#--------------------------------------------------------

FILE(GLOB SRC
  main.cpp)
  
ADD_EXECUTABLE(testBearingMinMaxToPoly ${SRC})
   				   
TARGET_LINK_LIBRARIES(testBearingMinMaxToPoly
  geometry
  mbutil
  m)

