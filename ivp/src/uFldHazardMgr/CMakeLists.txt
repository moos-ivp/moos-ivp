#--------------------------------------------------------
# The CMakeLists.txt for:                       uFldHazardMgr
# Author(s):                              
#--------------------------------------------------------

SET(SRC
  HazardMgr.cpp
  HazardMgr_Info.cpp
  main.cpp
)

ADD_EXECUTABLE(uFldHazardMgr ${SRC})

TARGET_LINK_LIBRARIES(uFldHazardMgr
   MOOS
   ufld_hazards
   geometry
   apputil
   mbutil
   m
   pthread)

