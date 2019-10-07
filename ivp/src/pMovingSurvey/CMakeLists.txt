#--------------------------------------------------------
# The CMakeLists.txt for:                   pMovingSurvey
# Author(s):                             Michael Benjamin  
#--------------------------------------------------------

SET(SRC
  MovingSurvey.cpp
  MovingSurvey_Info.cpp
  main.cpp
)

ADD_EXECUTABLE(pMovingSurvey ${SRC})

TARGET_LINK_LIBRARIES(pMovingSurvey
   ${MOOS_LIBRARIES}
   apputil
   geometry
   mbutil
   m
   pthread)

