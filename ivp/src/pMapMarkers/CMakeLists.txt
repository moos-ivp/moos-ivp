#--------------------------------------------------------
# The CMakeLists.txt for:                     pMapMarkers
# Author(s):                                Mike Benjamin
#--------------------------------------------------------

SET(SRC
  MapMarkers.cpp
  MapMarkers_Info.cpp
  main.cpp
)

ADD_EXECUTABLE(pMapMarkers ${SRC})

TARGET_LINK_LIBRARIES(pMapMarkers
   ${MOOS_LIBRARIES}
   apputil
   geometry
   mbutil
   m
   pthread)

