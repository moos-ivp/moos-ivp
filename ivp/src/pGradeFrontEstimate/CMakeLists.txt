#--------------------------------------------------------
# The CMakeLists.txt for:                       pGradeFrontEstimate
# Author(s):                              
#--------------------------------------------------------

# FILE(GLOB SRC *.cpp)

SET(SRC
  GradeFrontEstimate.cpp
  GradeFrontEstimate_Info.cpp
  main.cpp
)



ADD_EXECUTABLE(pGradeFrontEstimate ${SRC})

TARGET_LINK_LIBRARIES(pGradeFrontEstimate
   ${MOOS_LIBRARIES}
   mbutil
   m
   pthread)

