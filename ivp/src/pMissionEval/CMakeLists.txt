#--------------------------------------------------------
# The CMakeLists.txt for:                    pMissionEval
# Author(s):                                 Mike Benamin
#--------------------------------------------------------

SET(SRC
  MissionEval.cpp
  MissionEval_Info.cpp
  main.cpp
)

ADD_EXECUTABLE(pMissionEval ${SRC})

TARGET_LINK_LIBRARIES(pMissionEval
   ${MOOS_LIBRARIES}
   evalutil
   logic
   apputil
   mbutil
   m
   pthread)

