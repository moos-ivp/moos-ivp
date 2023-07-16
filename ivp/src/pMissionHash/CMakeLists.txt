#--------------------------------------------------------
# The CMakeLists.txt for:                    pMissionHash
# Author(s):                                Mike Benjamin
#--------------------------------------------------------

SET(SRC
  MissionHash_MOOSApp.cpp
  MissionHash_Info.cpp
  main.cpp
)

ADD_EXECUTABLE(pMissionHash ${SRC})

TARGET_LINK_LIBRARIES(pMissionHash
   ${MOOS_LIBRARIES}
   apputil
   mbutil
   m
   pthread)

