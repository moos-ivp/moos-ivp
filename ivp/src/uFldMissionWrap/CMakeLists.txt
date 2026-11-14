#--------------------------------------------------------
# The CMakeLists.txt for:                 uFldMissionWrap
# Author(s):                                Mike Benjamin
#--------------------------------------------------------

SET(SRC
  MissionWrap.cpp
  MissionWrap_Info.cpp
  MWrap.cpp
  main.cpp
)

ADD_EXECUTABLE(uFldMissionWrap ${SRC})

TARGET_LINK_LIBRARIES(uFldMissionWrap
   ${MOOS_LIBRARIES}
   apputil
   mbutil
   m
   pthread)
