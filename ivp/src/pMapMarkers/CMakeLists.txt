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
   apputil
   geometry
   mbutil
   pthread)

