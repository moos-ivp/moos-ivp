#--------------------------------------------------------
# The CMakeLists.txt for:                uFldHazardMetric
# Author(s):                             Michael Benjamin
#--------------------------------------------------------

# FILE(GLOB SRC *.cpp)

SET(SRC
  HazardMetric.cpp
  HazardMetric_Info.cpp
  main.cpp
)



ADD_EXECUTABLE(uFldHazardMetric ${SRC})

TARGET_LINK_LIBRARIES(uFldHazardMetric
   MOOS
   ufld_hazards
   geometry
   mbutil
   apputil
   m
   pthread)

