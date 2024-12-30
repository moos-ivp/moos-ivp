#--------------------------------------------------------
# The CMakeLists.txt for:                uFldCollObDetect
# Author(s):                                Mike Benjamin      
#--------------------------------------------------------

SET(SRC
  CollObDetect.cpp
  CollObDetect_Info.cpp
  main.cpp
)

ADD_EXECUTABLE(uFldCollObDetect ${SRC})

TARGET_LINK_LIBRARIES(uFldCollObDetect
   ${MOOS_LIBRARIES}
   ${MOOSGeodesy_LIBRARIES}
   apputil
   geodaid
   contacts
   geometry
   mbutil
   m
   pthread)

